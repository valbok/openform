#!/usr/bin/env python
import getopt, sys

opts, v = getopt.getopt(sys.argv[1:], "", [] )

r = eval( v[0] );

print "\
<update>\
  <widget name='input'>\
    <property name='text'>\
      <string>" + str( r ) + "</string>\
    </property>\
  </widget>\
</update>"
