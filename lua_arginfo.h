/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1148f900169e236df4a7a61313d82834b48afa6c */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_open, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_global_put, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_load_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_lua_load_string arginfo_lua_load_file

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_run, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lua_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(lua_open);
ZEND_FUNCTION(lua_global_put);
ZEND_FUNCTION(lua_load_file);
ZEND_FUNCTION(lua_load_string);
ZEND_FUNCTION(lua_run);
ZEND_FUNCTION(lua_close);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(lua_open, arginfo_lua_open)
	ZEND_FE(lua_global_put, arginfo_lua_global_put)
	ZEND_FE(lua_load_file, arginfo_lua_load_file)
	ZEND_FE(lua_load_string, arginfo_lua_load_string)
	ZEND_FE(lua_run, arginfo_lua_run)
	ZEND_FE(lua_close, arginfo_lua_close)
	ZEND_FE_END
};
