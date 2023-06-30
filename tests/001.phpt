--TEST--
Check if lua is loaded
--EXTENSIONS--
lua
--FILE--
<?php
echo 'The extension "lua" is available';
?>
--EXPECT--
The extension "lua" is available
