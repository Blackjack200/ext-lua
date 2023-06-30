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

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ void test1() */
PHP_FUNCTION(test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "lua");
}
/* }}} */

/* {{{ string test2( [ string $var ] ) */
PHP_FUNCTION(test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(lua)
{
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
    RETURN_LONG((long long) state);
}
/* }}} */

/* {{{ string lua_load( [] ) */
PHP_FUNCTION (lua_load) {
    char *file;
    size_t data_len;
    long long statePtr;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_LONG(statePtr)
            Z_PARAM_STRING(file, data_len)
            //
    ZEND_PARSE_PARAMETERS_END();
    struct lua_State *state = (struct lua_State *) statePtr;
    int stat = luaL_loadfile(state, file);
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
    luaL_openlibs(state);
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
PHP_MINFO_FUNCTION(lua)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "lua support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ lua_module_entry */
zend_module_entry lua_module_entry = {
	STANDARD_MODULE_HEADER,
	"lua",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(lua),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(lua),			/* PHP_MINFO - Module info */
	PHP_LUA_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LUA
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(lua)
#endif
