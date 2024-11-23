import cv2
import const
from signDetector import SignDetector
from control import RemoteControl

class RoadTraffic(object):
    signDetector: SignDetector = None
    remoteControl: RemoteControl = None
    enabled: bool = False
    signDetectorWasEnabled: bool = None

    def __init__(self, signDetector: SignDetector, remoteControl: RemoteControl):
        self.signDetector = signDetector
        self.remoteControl = remoteControl

    def switch(self, enabled: bool = None):
        if enabled == None:
            self.enabled = not self.enabled
        else:
            self.enabled = enabled
        
        if self.enabled:
            self.signDetectorWasEnabled = self.signDetector.enabled
            if not self.signDetectorWasEnabled:
                self.signDetector.switch(True)
            print("Движение по знакам включено")
        else:
            if self.signDetectorWasEnabled == False:
                self.signDetector.switch(False)
            self.signDetectorWasEnabled = None
            print("Движение по знакам выключено")

    def feed(self, img):
        detected = []
        if not self.enabled:
            return img, detected

        img, detected = self.signDetector.feed(img)

        if len(detected) > 0:
            sign = max(detected, key=lambda x: x.accuracy)
            x,y,w,h = sign.rect
            if w > 250:
                print(sign.name + ', width = ' + str(w))

        return img, detected
