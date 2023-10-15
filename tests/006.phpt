--TEST--
lua create object
--EXTENSIONS--
lua
--FILE--
<?php
class A extends stdClass {
public function hi(){
echo "HI\n";
}
public function hiStr(string $c="hihi"):string{
var_dump("from lua $c");
return "HiStr: ".$c;
}
}
$obj = new A();
$obj->a = 1;
var_dump($obj->a);

$ret = lua_open();
lua_global_put($ret, 'obj', $obj);

lua_do_string($ret, 'print(createObject("stdClass"))');
lua_close($ret);
?>
--EXPECT--
int(1)
1
2.0
float(2)
nil
string(13) "from lua hihi"
HiStr: hihi
nil
float(3)