#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "zend_types.h"
#include "zend_alloc.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

typedef struct {
    lua_State *L;
    HashTable *callbacks;
} LuaRuntime;

#define L_RT_ID(ptr) ((long long)(ptr))
#define L_RT(id) ((LuaRuntime*)(id))
#define L_RT_L(ptrOrId) ((L_RT(ptrOrId))->L)

LuaRuntime *lua_runtime_new() {
    LuaRuntime *a = emalloc(sizeof(LuaRuntime));
    a->L = luaL_newstate();
    a->callbacks = emalloc(sizeof(HashTable));
    zend_hash_init(a->callbacks, 32, NULL, ZVAL_PTR_DTOR, 0);
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
            lua_pushvalue(L, index);  // table
            lua_pushnil(L);  // first key
            while (lua_next(L, -2) != 0) {
                zval key, val;

                lua_pushvalue(L, -2);

                /* uses 'key' (at index -1) and 'value' (at index -2) */
                if (!php_lua_value(rt, -1, &key)) {
                    break;
                }
                if (!php_lua_value(rt, -2, &val)) {
                    zval_ptr_dtor(&key);
                    /* there is a warning already in php_lua_value */
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
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
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
            lua_pushlstring(L, ZSTR_VAL(str), ZSTR_LEN(str));
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
                return;
            }
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
