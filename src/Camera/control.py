# pip install pyserial

# Документация по serial:
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html

import serial 

PORT = "COM3" # 5 - левый, 3 - правый ближний, 6 - правый дальний
BAUDRATE = 9600
ser = serial.Serial()
telemetryEnabled = False
dist = str(0)
clrl = str(0)
clrr = str(0)

def connect():
    ser.port = PORT
    ser.timeout = 0
    ser.write_timeout = 0
    print("Подключение к пульту...")
    try:
        ser.open()
    except serial.SerialException:
        print("Не удалось подключиться к пульту")
        return
    print("Пульт подключен")
    telemetrySwitch(False)

def disconnect():
    telemetrySwitch(False)
    if ser.is_open:
        ser.close()
        print("Пульт отключен")

def telemetrySwitch(on=None):
    global telemetryEnabled
    if on == None:
        telemetryEnabled = not telemetryEnabled
    elif on == True:
        telemetryEnabled = True
    else:
        telemetryEnabled = False
   
    if not ser.is_open:
        if (telemetryEnabled):
            telemetryEnabled = False
            print("Пульт не подключен, невозможно включить телеметрию")
        return

    if (telemetryEnabled):
        ser.write(b'telemetry_start')
        print("Телеметрия включена")
    else:
        ser.write(b'telemetry_stop')
        print("Телеметрия выключена")

def readTelemetry():
    if (not telemetryEnabled) or (not ser.is_open):
        return False, 0, 0, 0
    global dist, clrl, clrr
    if ser.inWaiting():
        data = ser.readline().decode().strip()
        dist, clrl, clrr = data.split(',')
    return True, dist, clrl, clrr
