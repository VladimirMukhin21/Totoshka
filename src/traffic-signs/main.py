import cv2
from signDetector import SignDetector

cap = cv2.VideoCapture(0)
while (True):
    ret, img = cap.read()
    # img = cv2.imread("all.png")
    # img = cv2.imread("left-for-detect.png")
    # cv2.imshow('original', img)

    detector = SignDetector()
    img, detected = detector.detectAll(img)
    cv2.imshow('original', img)

    if cv2.waitKey(1)==ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
