import cv2
import numpy as np

path = "gentlemenclub.jpg"
cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")

def detect(img1):
    tmp = cv2.cv.CreateImage(cv2.cv.GetSize(img1),8,3)
    cv2.cv.CvtColor(img1,tmp,cv2.cv.CV_BGR2RGB)
    cv2.cv.CvtColor(tmp, tmp,cv2.cv.CV_BGR2RGB)

    img = np.asarray(cv2.cv.GetMat(tmp))

    #img = cv2.imread(path)
    #print img.shape

    print(type(img) == type(img1))
    rects = cascade.detectMultiScale(img, 1.3, 4, cv2.cv.CV_HAAR_SCALE_IMAGE, (20,20))

    if len(rects) == 0:
        return [], img
    rects[:, 2:] += rects[:, :2]
    return rects, img

def box(rects, img):
    for x1, y1, x2, y2 in rects:
        cv2.rectangle(img, (x1, y1), (x2, y2), (127, 255, 0), 2)
    cv2.cv.ShowImage("w1", cv2.cv.fromarray(img))
    #cv2.cv.ShowImage("w1", cv2.cv.QueryFrame(cam))
    #cv2.imwrite('detected.jpg', img);
    cv2.cv.WaitKey(0)


cv2.namedWindow("w1", (cv2.CV_WINDOW_AUTOSIZE))
cam = cv2.cv.CaptureFromCAM(0)

while True:
    pic = cv2.cv.QueryFrame(cam)
    print type(pic)
    rects, img = detect(pic)
    box(rects, img)

cv2.cv.destroyWindow("w1")