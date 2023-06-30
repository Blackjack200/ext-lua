/* lua extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_lua.h"
#include "lua_arginfo.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
#include "ext/spl/spl_exceptions.h"
#include "zend_exceptions.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
    ZEND_PARSE_PARAMETERS_START(0, 0) \
    ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION (lua) {
#if defined(ZTS) && defined(COMPILE_DL_LUA)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}
/* }}} */

/* {{{ string lua_open( [] ) */
PHP_FUNCTION (lua_open) {
    ZEND_PARSE_PARAMETERS_NONE();
    struct lua_State *state = luaL_newstate();
    luaL_openlibs(state);
    RETURN_LONG((long long) state);
}

/* }}} */
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

static inline void lua_push_php_value(struct lua_State *state, zval *val) {
    zend_string *str;
    switch (Z_TYPE_P(val)) {
        case IS_UNDEF:
        case IS_NULL:
            lua_pushnil(state);
            break;
        case IS_LONG:
            lua_pushinteger(state, zval_get_long(val));
            break;
        case IS_TRUE:
        case IS_FALSE:
            lua_pushboolean(state, Z_TYPE_P(val) == IS_TRUE);
            break;
        case IS_DOUBLE:
            lua_pushnumber(state, zval_get_double(val));
            break;
        case IS_STRING:
            str = Z_STR_P(val);
            lua_pushlstring(state, ZSTR_VAL(str), ZSTR_LEN(str));
            break;
        case IS_ARRAY:
            lua_newtable(state);
            HashTable *ht = Z_ARRVAL_P(val);
            zval *v;
            zend_ulong longkey;
            zend_string *key;
            ZEND_HASH_FOREACH_KEY_VAL_IND(ht, longkey, key, v){
                lua_push_php_value(state, v);
                if (key) {
                    lua_setfield(state, -2, ZSTR_VAL(key));
                } else {
                    lua_setfield(state, -2, ZSTR_VAL(zend_u64_to_str(longkey)));
                }
            }ZEND_HASH_FOREACH_END();
            break;
        case IS_OBJECT:
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

ZEND_FUNCTION(lua_global_put) {
    char *key;
    size_t key_len;
    long long statePtr;
    zval *val;

    ZEND_PARSE_PARAMETERS_START(3, 3)
            Z_PARAM_LONG(statePtr)
            Z_PARAM_STRING(key, key_len)
            Z_PARAM_ZVAL(val)
            //
    ZEND_PARSE_PARAMETERS_END();
    struct lua_State *state = (struct lua_State *) statePtr;
    lua_push_php_value(state, val);
    lua_setglobal(state, key);
}

/* {{{ string lua_load_file( [] ) */
PHP_FUNCTION (lua_load_file) {
    char *data;
    size_t data_len;
    long long statePtr;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(statePtr)
            Z_PARAM_STRING(data, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    struct lua_State *state = (struct lua_State *) statePtr;
    int stat = luaL_loadfile(state, data);
    RETURN_BOOL(stat);
}
/* }}}*/

/* {{{ string lua_load_string( [] ) */
PHP_FUNCTION (lua_load_string) {
    char *data;
    size_t data_len;
    long long statePtr;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(statePtr)
            Z_PARAM_STRING(data, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    struct lua_State *state = (struct lua_State *) statePtr;
    int stat = luaL_loadstring(state, data);
    RETURN_BOOL(stat);
}
/* }}}*/

/* {{{ string lua_load( [] ) */
PHP_FUNCTION (lua_run) {
    long long statePtr;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(statePtr)
            //
    ZEND_PARSE_PARAMETERS_END();
    struct lua_State *state = (struct lua_State *) statePtr;
    int stat = lua_pcall(state, 0, 0, 0);
    RETURN_BOOL(stat);
}
/* }}}*/

/* {{{ string lua_close( [] ) */
PHP_FUNCTION (lua_close) {
    long long statePtr;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(statePtr)
            //
    ZEND_PARSE_PARAMETERS_END();
    struct lua_State *state = (struct lua_State *) statePtr;
    lua_close(state);
}
/* }}}*/

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION (lua) {
    php_info_print_table_start();
    php_info_print_table_header(2, "lua support", "enabled");
    php_info_print_table_end();
}
/* }}} */

/* {{{ lua_module_entry */
zend_module_entry lua_module_entry = {
        STANDARD_MODULE_HEADER,
        "lua",                    /* Extension name */
        ext_functions,                    /* zend_function_entry */
        NULL,                            /* PHP_MINIT - Module initialization */
        NULL,                            /* PHP_MSHUTDOWN - Module shutdown */
        PHP_RINIT(lua),            /* PHP_RINIT - Request initialization */
        NULL,                            /* PHP_RSHUTDOWN - Request shutdown */
        PHP_MINFO(lua),            /* PHP_MINFO - Module info */
        PHP_LUA_VERSION,        /* Version */
        STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LUA
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(lua)
#endif
