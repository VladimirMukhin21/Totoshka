import cv2
import const

class EnabledModes:
    modes = []

    def switch(self, mode, enabled:bool = None):
        if mode == '':
            return

        if enabled == True:
            if mode not in self.modes:
                self.modes.append(mode)
        elif enabled == False:
            if mode in self.modes:
                self.modes.remove(mode)
        else:
            if mode in self.modes:
                self.modes.remove(mode)
            else:
                self.modes.append(mode)
        self.modes.sort()

    def show(self, img):
        cnt = len(self.modes)
        rowHeight = 11
        deltaY = 300 - rowHeight*cnt + 4
        for i in range(cnt):
            point = (const.x(369), const.y(deltaY + i*rowHeight))
            cv2.putText(img, self.modes[i], point, const.FONT, 0.5, color=const.RED)
