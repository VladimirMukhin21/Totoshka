# pip3 install opencv-python qrcode pyzbar numpy

from pyzbar import pyzbar
import cv2

def draw_barcode(decoded, image):
    n_points = len(decoded.polygon)
    for i in range(n_points):
        image = cv2.line(image, decoded.polygon[i], decoded.polygon[(i+1) % n_points], color=(0, 0, 255), thickness=5)
    # cv2.rectangle(
    #     image,
    #     (decoded.rect.left, decoded.rect.top), 
    #     (decoded.rect.left + decoded.rect.width, decoded.rect.top + decoded.rect.height),
    #     color = (0, 0, 255),
    #     thickness = 2)

def decode(image):
    decoded_objects = pyzbar.decode(image)
    for obj in decoded_objects:
        draw_barcode(obj, image)
        qr = obj.data.decode()
        capturedQrs.add(qr)
        print("QR:", qr, "  HIST:",  ", ".join(capturedQrs))

def x(x):
    return int(width*scale*x/400/100)
def y(y):
    return int(height*scale*y/300/100)

def draw_guides(image):
    thickness=1
    match guideMode:
        case 1:
            color=(0,255,0)
            # cv2.putText(image, "guides mode: 1", (x(5),y(10)), cv2.FONT_HERSHEY_PLAIN, 1, color, thickness=1, lineType=cv2.LINE_4)
            cv2.line(image, (x(30), y(250)), (x(370), y(250)), color, thickness) # горизонт
            cv2.line(image, (x(200), y(240)), (x(200), y(300)), color, thickness) # центр верт
            cv2.line(image, (x(0), y(300)), (x(70), y(230)), color, thickness) # наклон лев
            cv2.line(image, (x(400), y(300)), (x(330), y(230)), color, thickness) # наклон прав

def change_scale(delta):
    global scale
    global size
    scale += delta
    scale = max(scale, 30)
    scale = min(scale, 200)
    size = (int(width*scale/100), int(height*scale/100))
    print("scale:", scale, "%,  image size:", size[0], "x", size[1])

capturedQrs = set()
guideMode = 1
if __name__ == "__main__":
    cap = cv2.VideoCapture()
    cap.open(0) # номер камеры
    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    scale = 100
    change_scale(0)
    # print(width, "x", height)
    while True:
        readed, frame = cap.read()
        if readed:
            decode(frame)
            frame = cv2.resize(frame, size, interpolation=cv2.INTER_AREA)
            draw_guides(frame)
            # cv2.putText(frame, 'Totoshka', (x(5),y(20)), cv2.FONT_HERSHEY_SCRIPT_SIMPLEX, 1, color=(0,255,0), thickness=1, lineType=cv2.LINE_AA)
            cv2.imshow("TOTOSHKA", frame)
        key = cv2.waitKey(1)
        # print(key)
        if key >= ord("0") and key <= ord("9"):
            guideMode = key - ord("0")
            print("guideMode:", guideMode)
        if key == ord("+"):
            change_scale(10)
        if key == ord("-"):
            change_scale(-10)
        if key == 27: # Esc => exit
            break
