import cv2

FONT = cv2.FONT_HERSHEY_COMPLEX # только этот шрифт содержит русские буквы
RED = (0,0,255)
GREEN = (0,255,0)
BLUE = (255,0,0)
BLACK = (0,0,0)
YELLOW = (0,255,255)

width = 0
height = 0

def x(x):
    return int(width*x/400)
def y(y):
    return int(height*y/300)
