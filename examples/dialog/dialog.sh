#!/bin/bash
an=`./dialog.ui`
if [ $an = 'yes' ]; then
    echo 'There should be real installation of something very cool'
else
    echo 'Ok, we will do nothing...'
fi
