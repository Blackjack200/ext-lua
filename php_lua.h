/* lua extension for PHP */

#ifndef PHP_LUA_H
# define PHP_LUA_H

extern zend_module_entry lua_module_entry;
# define phpext_lua_ptr &lua_module_entry

# define PHP_LUA_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_LUA)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_LUA_H */
