#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "zend_types.h"
#include "zend_alloc.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "zend_interfaces.h"

typedef struct {
    lua_State *L;
    HashTable *callbacks;
} LuaRuntime;

#define L_RT_ID(ptr) ((long long)(ptr))
#define L_RT(id) ((LuaRuntime*)(id))
#define L_RT_L(ptrOrId) ((L_RT(ptrOrId))->L)

void register_builtin_lua_function(LuaRuntime *rt);

LuaRuntime *lua_runtime_new() {
    LuaRuntime *a = emalloc(sizeof(LuaRuntime));
    a->L = luaL_newstate();
    a->callbacks = emalloc(sizeof(HashTable));
    zend_hash_init(a->callbacks, 32, NULL, ZVAL_PTR_DTOR, 0);
    register_builtin_lua_function(a);
    return a;
}

void lua_runtime_free(LuaRuntime *r) {
    lua_close(r->L);
    zend_hash_destroy(r->callbacks);
    efree(r->callbacks);
    efree(r);
}

static inline zend_string *get_debug_type(zval *arg) {
    const char *name;
    switch (Z_TYPE_P(arg)) {
        case IS_NULL:
            return (ZSTR_KNOWN(ZEND_STR_NULL_LOWERCASE));
        case IS_FALSE:
        case IS_TRUE:
            return (ZSTR_KNOWN(ZEND_STR_BOOL));
        case IS_LONG:
            return (ZSTR_KNOWN(ZEND_STR_INT));
        case IS_DOUBLE:
            return (ZSTR_KNOWN(ZEND_STR_FLOAT));
        case IS_STRING:
            return (ZSTR_KNOWN(ZEND_STR_STRING));
        case IS_ARRAY:
            return (ZSTR_KNOWN(ZEND_STR_ARRAY));
        case IS_OBJECT:
            if (Z_OBJ_P(arg)->ce->ce_flags & ZEND_ACC_ANON_CLASS) {
                name = ZSTR_VAL(Z_OBJ_P(arg)->ce->name);
                return (zend_string_init(name, strlen(name), 0));
            } else {
                return (Z_OBJ_P(arg)->ce->name);
            }
        case IS_RESOURCE:
            name = zend_rsrc_list_get_rsrc_type(Z_RES_P(arg));
            if (name) {
                return (zend_strpprintf(0, "resource (%s)", name));
            } else {
                return (ZSTR_KNOWN(ZEND_STR_CLOSED_RESOURCE));
            }
        default:
            return (ZSTR_KNOWN(ZEND_STR_UNKNOWN));
    }
}

zval *php_lua_value(LuaRuntime *rt, int index, zval *rv) {
    lua_State *L = L_RT_L(rt);
    switch (lua_type(L, index)) {
        case LUA_TNIL:
            ZVAL_NULL(rv);
            break;
        case LUA_TBOOLEAN:
            ZVAL_BOOL(rv, lua_toboolean(L, index));
            break;
        case LUA_TNUMBER:
            ZVAL_DOUBLE(rv, lua_tonumber(L, index));
            break;
        case LUA_TSTRING: {
            char *val = NULL;
            size_t len = 0;

            val = (char *) lua_tolstring(L, index, &len);
            ZVAL_STRINGL(rv, val, len);
        }
            break;
        case LUA_TTABLE:
            array_init(rv);
            lua_pushvalue(L, index);
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                zval key, val;

                lua_pushvalue(L, -2);

                if (!php_lua_value(rt, -1, &key)) {
                    break;
                }
                if (!php_lua_value(rt, -2, &val)) {
                    zval_ptr_dtor(&key);
                    break;
                }

                switch (Z_TYPE(key)) {
                    case IS_DOUBLE:
                    case IS_LONG:
                        add_index_zval(rv, Z_DVAL(key), &val);
                        break;
                    case IS_STRING:
                        add_assoc_zval(rv, Z_STRVAL(key), &val);
                        zval_ptr_dtor(&key);
                        break;
                    case IS_ARRAY:
                    case IS_OBJECT:
                    default:
                        break;
                }
                lua_pop(L, 2);
            }
            lua_pop(L, 1);
            break;
        case LUA_TUSERDATA:
            if (lua_getmetatable(L, index)) {
                lua_pop(L, 1);
                rv = *(zval **) lua_touserdata(L, index);
                break;
            }
        case LUA_TFUNCTION:
        case LUA_TTHREAD:
        case LUA_TLIGHTUSERDATA:
        default:
            zend_throw_exception_ex(
                    spl_ce_InvalidArgumentException,
                    0,
                    "unsupported type '%s' for php",
                    lua_typename(L, lua_type(L, index))
            );
            ZVAL_NULL(rv);
            return NULL;
    }
    return rv;
}

void lua_push_php_value(LuaRuntime *rt, zval *val);

int luaIndexMetaMethod(lua_State *L) {
    zend_object *phpObj = Z_OBJ_P(*(zval **) lua_touserdata(L, 1));
    LuaRuntime *rt = L_RT((long long) lua_tonumber(L, lua_upvalueindex(1)));
    const char *key = luaL_checkstring(L, 2);
    zend_string *str = zend_string_init(key, strlen(key), 0);
    zval *prop = zend_hash_find(phpObj->handlers->get_properties(phpObj), str);
    if (prop == NULL) {
        lua_pushnil(rt->L);
    } else {
        lua_push_php_value(rt, prop);
    }
    zend_string_release(str);
    return 1;
}

int luaNewIndexMetaMethod(lua_State *L) {
    zend_object *phpObj = Z_OBJ_P(*(zval **) lua_touserdata(L, 1));
    LuaRuntime *rt = L_RT((long long) lua_tonumber(L, lua_upvalueindex(1)));

    const char *key = luaL_checkstring(L, 2);
    zval value;
    ZVAL_UNDEF(&value);
    value = *php_lua_value(rt, 3, &value);
    zend_update_property(phpObj->ce, phpObj, key, strlen(key), &value);
    zval_ptr_dtor(&value);
    return 0;
}

int luaGCMetaMethod(lua_State *L) {
    zend_object *phpObj = Z_OBJ_P(*(zval **) lua_touserdata(L, 1));
    LuaRuntime *rt = L_RT((long long) lua_tonumber(L, lua_upvalueindex(1)));
    GC_TRY_DELREF(phpObj);
    return 0;
}

int luaCallMetaMethod(lua_State *L) {
    zval *zp = *(zval **) lua_touserdata(L, 1);
    zend_object *phpObj = Z_OBJ_P(zp);
    LuaRuntime *rt = L_RT((long long) lua_tonumber(L, lua_upvalueindex(1)));
    const char *methodName = luaL_checkstring(L, 2);

    int arg_num = lua_gettop(L) - 2;
    zval *params = emalloc(sizeof(zval) * arg_num);

    for (int i = 0; i < arg_num; i++) {
        php_lua_value(rt, -(arg_num - i), &params[i]);
    }

    zval retval;

    zend_string *zstr = zend_string_init(methodName, strlen(methodName), 0);

    zend_result result = zend_call_method_if_exists(phpObj, zstr, &retval, arg_num, params);
    if (result == FAILURE) {
        const char *errorMsg = lua_pushfstring(L, "Call method failed");
        luaL_error(L, errorMsg);
        return 0;
    }
    zend_string_release(zstr);

    lua_push_php_value(rt, &retval);
    zval_ptr_dtor(&retval);
    for (int i = 0; i < arg_num; i++) {
        zval_ptr_dtor(&params[i]);
    }
    efree(params);
    ZVAL_OBJ(zp, phpObj);
    return 1;
}

void createProxiedPHPObject(LuaRuntime *rt, zval *phpObj) {
    lua_State *L = L_RT_L(rt);
    zval **ptr = (zval **) lua_newuserdata(L, sizeof(zval *));
    *ptr = phpObj;

    lua_newtable(L);

    lua_pushnumber(L, L_RT_ID(rt));
    lua_pushcclosure(L, luaIndexMetaMethod, 1);
    lua_setfield(L, -2, "__index");

    lua_pushnumber(L, L_RT_ID(rt));
    lua_pushcclosure(L, luaNewIndexMetaMethod, 1);
    lua_setfield(L, -2, "__newindex");

    lua_pushnumber(L, L_RT_ID(rt));
    lua_pushcclosure(L, luaGCMetaMethod, 1);
    lua_setfield(L, -2, "__gc");

    lua_pushnumber(L, L_RT_ID(rt));
    lua_pushcclosure(L, luaCallMetaMethod, 1);
    lua_setfield(L, -2, "__call");

    lua_setmetatable(L, -2);
}

int luaCreateObject(lua_State *L) {
    LuaRuntime *rt = L_RT((long long) lua_tonumber(L, lua_upvalueindex(1)));
    zend_class_entry *ce;

    if (lua_isstring(L, 1)) {
        const char *className = lua_tostring(L, 1);
        zend_string *zstr = zend_string_init(className, strlen(className), 0);
        ce = zend_lookup_class(zstr);
        zend_string_release(zstr);
        if (ce == NULL) {
            const char *errorMsg = lua_pushfstring(L, "Class %s not found", className);
            luaL_error(L, errorMsg);
            return 0;
        }
    } else {
        luaL_error(L, "Invalid argument: expected string");
        return 0;
    }
    zval phpObj;
    object_init_ex(&phpObj, ce);
    createProxiedPHPObject(rt, &phpObj);
    zval_ptr_dtor(&phpObj);
    return 1;
}

void register_builtin_lua_function(LuaRuntime *rt) {
    lua_pushnumber(rt->L, L_RT_ID(rt));
    lua_pushcclosure(rt->L, luaCreateObject, 1);
    lua_setglobal(rt->L, "createObject");
}

int php_lua_call_callback(lua_State *L) {
    zval retval;
    int idx = lua_tonumber(L, lua_upvalueindex(1));
    LuaRuntime *rt = L_RT((long long) lua_tonumber(L, lua_upvalueindex(2)));
    zval *func = zend_hash_index_find(rt->callbacks, idx);

    if (!zend_is_callable(func, 0, NULL)) {
        return 0;
    } else {
        int arg_num = lua_gettop(L);
        zval *params = emalloc(sizeof(zval) * arg_num);

        for (int i = 0; i < arg_num; i++) {
            php_lua_value(rt, -(arg_num - i), &params[i]);
        }

        call_user_function(EG(function_table), NULL, func, &retval, arg_num, params);
        lua_push_php_value(rt, &retval);

        for (int i = 0; i < arg_num; i++) {
            zval_ptr_dtor(&params[i]);
        }
        efree(params);
        zval_ptr_dtor(&retval);
        return 1;
    }
}

void lua_push_php_value(LuaRuntime *rt, zval *val) {
    lua_State *L = L_RT_L(rt);
    zend_string *str;
    switch (Z_TYPE_P(val)) {
        case IS_UNDEF:
        case IS_NULL:
            lua_pushnil(L);
            break;
        case IS_LONG:
            lua_pushinteger(L, zval_get_long(val));
            break;
        case IS_TRUE:
        case IS_FALSE:
            lua_pushboolean(L, Z_TYPE_P(val) == IS_TRUE);
            break;
        case IS_DOUBLE:
            lua_pushnumber(L, zval_get_double(val));
            break;
        case IS_STRING:
            str = Z_STR_P(val);
            lua_pushstring(L, ZSTR_VAL(str));
            break;
        case IS_ARRAY:
            lua_newtable(L);
            HashTable *ht = Z_ARRVAL_P(val);
            zval *v;
            zend_ulong longkey;
            zend_string *key;
            ZEND_HASH_FOREACH_KEY_VAL_IND(ht, longkey, key, v)
                    {
                        lua_push_php_value(rt, v);
                        if (key) {
                            lua_setfield(L, -2, ZSTR_VAL(key));
                        } else {
                            lua_setfield(L, -2, ZSTR_VAL(zend_u64_to_str(longkey)));
                        }
                    }
            ZEND_HASH_FOREACH_END();
            break;
        case IS_OBJECT:
            if (zend_is_callable(val, 0, NULL)) {
                lua_pushnumber(L, zend_hash_num_elements(rt->callbacks));
                lua_pushnumber(L, L_RT_ID(rt));
                lua_pushcclosure(L, php_lua_call_callback, 2);
                Z_ADDREF_P(val);
                zend_hash_index_add(rt->callbacks, zend_hash_num_elements(rt->callbacks), val);
            } else {
                Z_ADDREF_P(val);
                createProxiedPHPObject(rt, val);
            }
            break;
        case IS_RESOURCE:
        default:
            //TODO
            zend_throw_exception_ex(
                    spl_ce_InvalidArgumentException,
                    0,
                    "Unsupported value type: %s",
                    ZSTR_VAL(get_debug_type(val))
            );
            return;
    }
}


