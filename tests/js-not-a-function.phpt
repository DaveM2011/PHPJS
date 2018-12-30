--TEST--
Convert not found functions in Javascript as an PHP exception
--FILE--
<?php 
$js = new JS;
$js->foobar();
--EXPECTF--
Fatal error: Uncaught JSException: foobar() is not a javascript function in %s:%d
Stack trace:
#0 %s(%d): JS->__call('foobar', Array)
#1 {main}
  thrown in %s on line %d

