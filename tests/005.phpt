--TEST--
lua object basic test
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

lua_do_string($ret, 'print(obj.a)');
lua_do_string($ret, 'obj.a=2');
lua_do_string($ret, 'print(obj.a)');
var_dump($obj->a);

lua_do_string($ret, 'print(obj.b)');
lua_do_string($ret, 'obj.b=3');
lua_do_string($ret, 'print(obj("hiStr"))');
lua_do_string($ret, 'print(obj.hiStr)');
var_dump($obj->b);

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