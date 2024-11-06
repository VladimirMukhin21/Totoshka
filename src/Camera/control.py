# pip install pyserial

# Документация по serial:
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html

import serial 
from datetime import datetime

class RemoteControl(object):
    PORT = "COM4" # COM4   # 5 - левый, 3 - правый ближний, 6 - правый дальний
    BAUDRATE = 115200
    ser = serial.Serial()
    telemetryEnabled = False
    dist = 0
    clrl = 0
    clrr = 0
    lastSendDriveTime = datetime.now()

    def connect(self):
        self.ser.port = self.PORT
        self.ser.baudrate = self.BAUDRATE
        self.ser.timeout = 0
        self.ser.write_timeout = 0
        print("Подключение к пульту...")
        try:
            self.ser.open()
        except serial.SerialException:
            print("Не удалось подключиться к пульту")
            return
        print("Пульт подключен")
        self.telemetrySwitch(False)

    def disconnect(self):
        self.telemetrySwitch(False)
        if self.ser.is_open:
            self.ser.close()
            print("Пульт отключен")

    def telemetrySwitch(self, on=None):
        if on == None:
            self.telemetryEnabled = not self.telemetryEnabled
        elif on == True:
            self.telemetryEnabled = True
        else:
            self.telemetryEnabled = False
    
        if not self.ser.is_open:
            if (self.telemetryEnabled):
                self.telemetryEnabled = False
                print("Пульт не подключен, невозможно включить телеметрию")
            return

        if (self.telemetryEnabled):
            self.ser.write(b'\x010000')
            print("Телеметрия включена")
        else:
            self.ser.write(b'\x020000')
            print("Телеметрия выключена")

    def readTelemetry(self):
        if (not self.telemetryEnabled) or (not self.ser.is_open):
            return False, 0, 0, 0
        if self.ser.inWaiting():
            data = self.ser.readline().decode().strip()
            self.dist, self.clrl, self.clrr = map(int, data.split(','))
        return True, self.dist, self.clrl, self.clrr

    def sendDriveCommand(self, speed, turn):
        """Параметры speed и turn должны быть в диапазоне [0-255], 127 - среднее значение, т.е. стоять."""
        if not self.ser.is_open:
            return
        
        now = datetime.now()
        delta = now - self.lastSendDriveTime
        if delta.microseconds < 50000: # 50ms
            return
        
        self.ser.write(b'\x03' + speed.to_bytes(1, byteorder='big') + turn.to_bytes(1, byteorder='big'))
        self.lastSendDriveTime = now
        print("drive(" + str(speed) + ", " + str(turn) + ")")
