#!/bin/bash
if [ $1 -ge 100 ]; then
    echo '<update>
        <widget name="centralwidget">
            <widget name="labelStep3">
                <property name="text">
                    <string>OMG, we have found and deleted 32 videos of GAY porn.</string>
                </property>
            </widget>
            <widget name="pushStep3Exit">
                <property name="visible">
                    <bool>true</bool>
                </property>
                <property name="text">
                    <string>I am not surprised</string>
                </property>
            </widget>
            <widget name="pushStep3Stop">
                <property name="visible">
                    <bool>false</bool>
                </property>
            </widget>
        </widget>
    </update>'
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
sleep 0.1
