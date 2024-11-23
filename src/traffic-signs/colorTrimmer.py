import cv2

def nothing(void):
    pass

def onMouse(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        if imgCopy.any():
            h, w, _ = img.shape
            if x >= 0 and x <= w and y >= 0 and y <= h:
                hsv = imgCopy[y,x]
                print(hsv)
                delta = cv2.getTrackbarPos('delta', 'result')
                cv2.setTrackbarPos('minH', 'result', max(0, hsv[0]-delta))
                cv2.setTrackbarPos('maxH', 'result', min(179, hsv[0]+delta))
                cv2.setTrackbarPos('minS', 'result', max(0, hsv[1]-delta))
                cv2.setTrackbarPos('maxS', 'result', min(255, hsv[1]+delta))
                cv2.setTrackbarPos('minV', 'result', max(0, hsv[2]-delta))
                cv2.setTrackbarPos('maxV', 'result', min(255, hsv[2]+delta))
    elif event == cv2.EVENT_RBUTTONDOWN:
        cv2.setTrackbarPos('minH', 'result', 0)
        cv2.setTrackbarPos('maxH', 'result', 179)
        cv2.setTrackbarPos('minS', 'result', 0)
        cv2.setTrackbarPos('maxS', 'result', 255)
        cv2.setTrackbarPos('minV', 'result', 0)
        cv2.setTrackbarPos('maxV', 'result', 255)

cap = cv2.VideoCapture(0)

cv2.namedWindow('result')

cv2.createTrackbar('minH', 'result', 0, 179, nothing)
cv2.createTrackbar('maxH', 'result', 179, 179, nothing)

cv2.createTrackbar('minS', 'result', 0, 255, nothing)
cv2.createTrackbar('maxS', 'result', 255, 255, nothing)

cv2.createTrackbar('minV', 'result', 0, 255, nothing)
cv2.createTrackbar('maxV', 'result', 255, 255, nothing)

cv2.createTrackbar('delta', 'result', 40, 80, nothing)

cv2.setMouseCallback('result', onMouse)
imgCopy = None

while(True):
    ret, img = cap.read()
    # img = cv2.imread("all.png")
    # img = cv2.imread('samples/amox.png')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    imgCopy = img.copy()

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
