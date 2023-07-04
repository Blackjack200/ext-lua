--TEST--
lua basic test
--EXTENSIONS--
lua
--FILE--
<?php
$ret = lua_open();
lua_global_put($ret, 'data', ["e"=>['json'=> null]]);

lua_do_file($ret, 'json.lua');

lua_do_string($ret, 'json = require "json"');
lua_do_string($ret, 'print(json.encode(data))');

lua_close($ret);
?>
--EXPECT--
{"e":[]}