import cv2
import numpy as np

fileName = '20231130_091922.avi'
newFileName = 'new_' + fileName
cap = cv2.VideoCapture(fileName)

if not cap.isOpened():
    print("Error opening video stream or file")
    exit()

width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

codec = cv2.VideoWriter_fourcc(*'DIVX')
# codec = cv2.VideoWriter_fourcc(*'XVID')
# codec = cv2.VideoWriter_fourcc(*'MJPG') # файл больше раза в 2
file = cv2.VideoWriter(newFileName, codec, 25, (width, height))

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
    cv2.imshow('Frame', frame)
    file.write(frame)

    # если 1, то видео отображается ускорено, но в файл пишется нормально
    # для нормальной скорости отображения поставить 25
    key = cv2.waitKey(1)
    if key & 0xFF == ord('q'):  # Press Q on keyboard to exit
        break

cap.release()
file.release()
cv2.destroyAllWindows()
