#!/usr/bin/env php
<?php
$c = isset( $argv[1] ) ? $argv[1] : 0;
$result = 0;

eval( "\$result = " . $c . ";" );

print 
"<update>
  <widget name=\"input\">.
    <property name=\"text\">
      <string>" . $result . "</string>
    </property>
  </widget> 
</update>";

?>