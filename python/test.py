# pysunray test  (steer robot via Dabble App)
#
#  install pybind11 with:  
#     sudo apt install pybind11-dev 
#     pip install "pybind11[global]"
#
#  cmake .. 
#  cmake -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
#
# to run the python interpreter as sudo:
#  1. which python  =>    /home/alex/miniconda3/envs/py38/bin/python
#  2. sudo /home/alex/miniconda3/envs/py38/bin/python test.py

import sys
import os
sys.path.append("./build/")
import time
import pysunray as sun

print('--------------start------------')
#time.sleep(2.0)

# connect to Dabble App
dabble = sun._Dabble
gamepad = sun._GamePad

#ble = sun.BleUartServer()
ble = sun._SerialBLE
mot = sun._motor
driver = sun._robotDriver

#ble.begin(115200)
driver.begin()
dabble.begin('test')

# some default values
toolOn = False
maxLinearSpeed = 0.1
maxAngularSpeed = 0.1

buttonTimeout = 0
nextInfoTime = 0


# main loop
while True:
    '''if ble.available():
        s = ''
        while ble.available():
            s +=chr(ble.read())
        print(s)'''
    
    # get gamepad input
    dabble.processInput() 

    # get gamepad analog values (x, y)
    x = gamepad.getXaxisData()
    y = gamepad.getYaxisData()

    # decide the motor speed to send based on digital input buttons
    if gamepad.isUpPressed():
        linearSpeed = maxLinearSpeed
        angularSpeed = 0
    elif gamepad.isDownPressed():
        linearSpeed = -maxLinearSpeed
        angularSpeed = 0            
    elif gamepad.isLeftPressed():
        linearSpeed = 0
        angularSpeed = maxAngularSpeed        
    elif gamepad.isRightPressed():
        linearSpeed = 0
        angularSpeed = -maxAngularSpeed        
    else:
        # no gamepad direction (up/down/left/right) button pressed => use analog values (x, y)
        if time.time() > buttonTimeout:    
            if gamepad.isSquarePressed():
                print('isSquarePressed - shutdown system...')
                os.system('shutdown now')
            elif gamepad.isCirclePressed():
                print('isCirclePressed')
                toolOn = not toolOn
                buttonTimeout = time.time() + 0.5
            elif gamepad.isCrossPressed():
                print('isCrossPressed')
                maxLinearSpeed = 0.1
            elif gamepad.isTrianglePressed():
                print('isTrianglePressed')                
                maxLinearSpeed = 0.2

        linearSpeed = y / 6.0 * maxLinearSpeed
        angularSpeed = x / 6.0 * maxAngularSpeed 

    # print values to console
    if time.time() > nextInfoTime:
        nextInfoTime = time.time() + 0.5        
        print('linearSpeed', round(linearSpeed, 2), 'angularSpeed', round(angularSpeed, 2), 
              'toolOn', toolOn, 'x', round(x, 1), 'y', round(y, 1))

    # send speeds to motor
    mot.setLinearAngularSpeed(linearSpeed, angularSpeed, True)
    mot.setMowState(toolOn);   
    mot.run()
    driver.run()

    #print('.')
    time.sleep(0.01)



