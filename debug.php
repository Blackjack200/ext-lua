<?php
class A extends stdClass {
public $a;
public function hi(){
echo "HI\n";
}
public function xb(string $c):void{
echo "HiStr: $c\n";
}
}
$obj = new A();

$ret = lua_open();
lua_global_put($ret, 'obj', $obj);
var_dump($obj);
lua_do_string($ret, 'obj.a = createObject("stdClass")');
var_dump($obj);
var_dump($obj->a);
lua_close($ret);