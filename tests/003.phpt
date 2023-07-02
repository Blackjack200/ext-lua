--TEST--
lua basic test
--EXTENSIONS--
lua
--FILE--
<?php
$ret = lua_open();
lua_global_put($ret, 'hi', static fn()=>printf("hi\n"));
lua_global_put($ret, 'hi2', static fn()=>1.11);
lua_global_put($ret, 'hi3', static fn($p)=>'hi '.$p);
lua_global_put($ret, 'hi4', static fn()=>static fn()=>111);
lua_global_put($ret, 'read_file', static fn($x)=>file_get_contents($x));

lua_load_string($ret, 'hi()');
lua_run($ret);

lua_load_string($ret, 'print(hi2())');
lua_run($ret);

lua_load_string($ret, 'print(hi3("lua"))');
lua_run($ret);

lua_load_string($ret, 'print(type(hi4()))');
lua_run($ret);

lua_load_string($ret, 'print(hi4()())');
lua_run($ret);

lua_load_string($ret,'print(read_file("test.lua"))');
lua_run($ret);

lua_close($ret);
?>
--EXPECT--
hi
1.11
hi lua
function
111
print("Hello lua")