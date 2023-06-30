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
	'e' => [
		1 => 2,
		3 => 4,
		"a" => 'x',
	],
];
$ret = lua_open();
foreach ($vars as $name => $val) {
	lua_global_put($ret, $name, $val);
}

lua_load_file($ret, 'test.lua');
lua_run($ret);

lua_load_string($ret, "print('hello lua')");
lua_run($ret);

foreach ($vars as $name => $val) {
	lua_load_string($ret, <<<CODE
if(type($name)=="table") then
    print("table")
    print({$name}["a"])
else
    print($name)
end
CODE
	);
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
table
x