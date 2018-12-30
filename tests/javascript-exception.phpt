--TEST--
Simple excpetion tests
--FILE--
<?php 
$js = new JS;
$ret = $js->evaluate(<<<EOF
function someFunction() {
    throw new Error("foobar");
}
EOF
);
$js->someFunction();
--EXPECTF--
Fatal error: Uncaught JSException: Error: foobar in %s:%d
Stack trace:
#0 %s(%d): JS->__call('someFunction', Array)
#1 {main}
  thrown in %s on line %d

