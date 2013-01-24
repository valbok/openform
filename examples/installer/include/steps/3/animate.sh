#!/bin/bash
if [ $1 -ge 100 ]; then
    echo '<update></update>'
    exit 0;
fi

r=`expr $1 + 1`

echo '<update>
    <widget class="QMainWindow" name="MainWindow">
        <widget name="step3">
            <widget class="QWidget" name="progressStep3">
                <property name="value">
                    <number>'$r'</number>
                </property>
            </widget>
        </widget>
    </widget>
</update>'
sleep 0.2
