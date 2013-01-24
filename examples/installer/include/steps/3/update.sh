#!/bin/bash
echo '
<update>
    <widget class="QMainWindow" name="MainWindow">
        <widget class="QWidget" name="centralwidget">
            <widget class="QWidget" name="step2">
                <property name="visible">
                    <bool>false</bool>
                </property>
            </widget>
            <widget name="step1">
                <property name="visible">
                    <bool>false</bool>
                </property>
            </widget>
            <widget name="step3">
                <property name="visible">
                    <bool>true</bool>
                </property>
                <widget class="QWidget" name="progressStep3">
                    <property name="value">
                        <number>10</number>
                    </property>
                </widget>
            </widget>
        </widget>
    </widget>
</update>'
