--TEST--
test1() Basic test
--EXTENSIONS--
lua
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension lua is loaded and working!
NULL
