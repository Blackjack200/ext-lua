--TEST--
lua basic test
--EXTENSIONS--
lua
--FILE--
<?php
$ret = lua_open();
lua_load($ret,'test.lua');
lua_run($ret);
lua_close($ret);
?>
--EXPECT--
Hello lua