/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8df70cd24294e2c32b3dc57149e31b1170c1bbb3 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_open, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_global_put, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_load_file, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_lua_load_string arginfo_lua_load_file

#define arginfo_lua_do_file arginfo_lua_load_file

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_do_string, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_run, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_lua_close arginfo_lua_run


ZEND_FUNCTION(lua_open);
ZEND_FUNCTION(lua_global_put);
ZEND_FUNCTION(lua_load_file);
ZEND_FUNCTION(lua_load_string);
ZEND_FUNCTION(lua_do_file);
ZEND_FUNCTION(lua_do_string);
ZEND_FUNCTION(lua_run);
ZEND_FUNCTION(lua_close);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(lua_open, arginfo_lua_open)
	ZEND_FE(lua_global_put, arginfo_lua_global_put)
	ZEND_FE(lua_load_file, arginfo_lua_load_file)
	ZEND_FE(lua_load_string, arginfo_lua_load_string)
	ZEND_FE(lua_do_file, arginfo_lua_do_file)
	ZEND_FE(lua_do_string, arginfo_lua_do_string)
	ZEND_FE(lua_run, arginfo_lua_run)
	ZEND_FE(lua_close, arginfo_lua_close)
	ZEND_FE_END
};
