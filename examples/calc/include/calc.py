#!/usr/bin/env python
import getopt, sys

opts, v = getopt.getopt( sys.argv[1:], "", [] )

try:
   value = v[0]
except IndexError:
    sys.exit();

r = eval( value );

print "\
<update>\
  <widget name='input'>\
    <property name='text'>\
      <string>" + str( r ) + "</string>\
    </property>\
  </widget>\
</update>"
