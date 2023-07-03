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

#include "binding.c"

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

#define CHECK_LUA_RET() if (stat != LUA_OK) { \
const char *errorMsg = lua_tostring(rt->L, -1); \
lua_pop(rt->L, 1); \
zend_throw_exception_ex( \
        spl_ce_RuntimeException, \
0, \
"%s", \
errorMsg \
); \
}

/* {{{ string lua_open( [] ) */
PHP_FUNCTION (lua_open) {
    ZEND_PARSE_PARAMETERS_NONE();
    LuaRuntime *rt = lua_runtime_new();
    luaL_openlibs(L_RT_L(rt));
    RETURN_LONG(L_RT_ID(rt));
}

ZEND_FUNCTION(lua_global_put) {
    char *key;
    size_t key_len;
    long long id;
    zval *val;

    ZEND_PARSE_PARAMETERS_START(3, 3)
            Z_PARAM_LONG(id)
            Z_PARAM_STRING(key, key_len)
            Z_PARAM_ZVAL(val)
            //
    ZEND_PARSE_PARAMETERS_END();
    LuaRuntime *rt = L_RT(id);
    lua_push_php_value(rt, val);
    lua_setglobal(L_RT_L(rt), key);
}

/* {{{ string lua_load_file( [] ) */
PHP_FUNCTION (lua_load_file) {
    char *data;
    size_t data_len;
    long long id;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(id)
            Z_PARAM_STRING(data, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    LuaRuntime *rt = L_RT(id);
    int stat = luaL_loadfile(L_RT_L(rt), data);
    CHECK_LUA_RET()
}
/* }}}*/

/* {{{ string lua_load_string( [] ) */
PHP_FUNCTION (lua_load_string) {
    char *data;
    size_t data_len;
    long long id;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(id)
            Z_PARAM_STRING(data, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    LuaRuntime *rt = L_RT(id);
    int stat = luaL_loadstring(L_RT_L(rt), data);
    CHECK_LUA_RET()
}
/* }}}*/

/* {{{ string lua_do_string( [] ) */
PHP_FUNCTION (lua_do_string) {
    char *data;
    size_t data_len;
    long long id;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(id)
            Z_PARAM_STRING(data, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    LuaRuntime *rt = L_RT(id);
    int stat = luaL_dostring(L_RT_L(rt), data);
    CHECK_LUA_RET()
}
/* }}}*/

/* {{{ string lua_do_file( [] ) */
PHP_FUNCTION (lua_do_file) {
    char *data;
    size_t data_len;
    long long id;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(id)
            Z_PARAM_STRING(data, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    LuaRuntime *rt = L_RT(id);
    int stat = luaL_dofile(L_RT_L(rt), data);
    CHECK_LUA_RET()
}
/* }}}*/

/* {{{ string lua_load( [] ) */
PHP_FUNCTION (lua_run) {
    long long id;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(id)
            //
    ZEND_PARSE_PARAMETERS_END();
    LuaRuntime *rt = L_RT(id);
    int stat = lua_pcall(L_RT_L(rt), 0, 0, 0);
    CHECK_LUA_RET()
}
/* }}}*/

/* {{{ string lua_close( [] ) */
PHP_FUNCTION (lua_close) {
    long long id;
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(id)
            //
    ZEND_PARSE_PARAMETERS_END();
    lua_runtime_free(L_RT(id));
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
