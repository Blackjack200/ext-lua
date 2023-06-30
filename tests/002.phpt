--TEST--
lua basic test
--EXTENSIONS--
lua
--FILE--
<?php
$vars = [
'a' => 1,
'b' => true,
'c' => 1.14514,
'd' => "from php",
];
$ret = lua_open();
foreach($vars as $name => $val){
lua_global_put($ret,$name,$val);
}

lua_load_file($ret,'test.lua');
lua_run($ret);

lua_load_string($ret,"print('hello lua')");
lua_run($ret);

foreach($vars as $name => $val){
lua_load_string($ret,"print($name)");
lua_run($ret);
}
lua_close($ret);
?>
--EXPECT--
Hello lua
hello lua
1
true
1.14514
from php