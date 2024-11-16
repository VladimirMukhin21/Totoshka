import cv2

def nothing(void):
    pass

cap = cv2.VideoCapture(0)

cv2.namedWindow('result')

cv2.createTrackbar('minH', 'result', 0, 179, nothing)
cv2.createTrackbar('maxH', 'result', 179, 179, nothing)

cv2.createTrackbar('minS', 'result', 0, 255, nothing)
cv2.createTrackbar('maxS', 'result', 255, 255, nothing)

cv2.createTrackbar('minV', 'result', 0, 255, nothing)
cv2.createTrackbar('maxV', 'result', 255, 255, nothing)

while(True):
    # ret, frame = cap.read()
    # img = cv2.imread("all.png")
    img = cv2.imread('samples/amox.png')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # img = cv2.GaussianBlur(img, (19, 19), 0)

    minH = cv2.getTrackbarPos('minH', 'result')
    maxH = cv2.getTrackbarPos('maxH', 'result')

    minS = cv2.getTrackbarPos('minS', 'result')
    maxS = cv2.getTrackbarPos('maxS', 'result')

    minV = cv2.getTrackbarPos('minV', 'result')
    maxV = cv2.getTrackbarPos('maxV', 'result')

    mask = cv2.inRange(img, (minH,minS,minV), (maxH,maxS,maxV))
    cv2.imshow('mask', mask)

    # mask = cv2.erode(mask, None, iterations=2)
    # mask = cv2.dilate(mask, None, iterations=4)

    result = cv2.bitwise_and(img, img, mask = mask)
    
    imgShow = cv2.cvtColor(result, cv2.COLOR_HSV2BGR)
    cv2.imshow('result', imgShow)

    if cv2.waitKey(1)==ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
