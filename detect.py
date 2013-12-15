'''
TEST FILE:
Uses haarcascade's XML file to train CascadeClassifier to
  recognize faces.
Infinitely runs through video input and detects faces.
  Boxes the face and displays at relatively live speed.

--Mitchell
'''

'''
DEMOCODE:

In progress: Arduino Motor Control
'''

import cv2
import numpy as np
import serial
import time

TIME_BEGIN = time.time()

''' - Jas '''

#Imports the training data.
cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")

serial_on = True
serial_name = "/dev/ttyACM0"

def tick():
    global TIME_BEGIN
    TIME_BEGIN = time.time()

def tock(silent=False):
    global TIME_BEGIN
    if(silent):
        return time.time() - TIME_BEGIN
    delta = time.time() - TIME_BEGIN
    print delta
    return delta

def ticktock(fun):
    tick()
    fun
    tock()

def detect(imgin):
    '''
    Takes in an image (Webcam image format) and returns the
    Rectangle X and Y of the image
    '''
    tmp = cv2.cv.CreateImage(cv2.cv.GetSize(imgin),8,3) #creates image of the right size
    cv2.cv.CvtColor(imgin, tmp,cv2.cv.CV_BGR2RGB)
    cv2.cv.CvtColor(tmp, tmp,cv2.cv.CV_BGR2RGB) #Puts colors into the image

    img = np.asarray(cv2.cv.GetMat(tmp)) #Converts to proper format

    #Creates the rectangles that border the faces
    rects = cascade.detectMultiScale(img, 1.3, 4, 
            cv2.cv.CV_HAAR_SCALE_IMAGE, (20,20))
            
    if len(rects) == 0:
        return [], img
    rects[:, 2:] += rects[:, :2]
    return rects, img

def box(rects, img):
    global last_dx, last_dy

    closest = 100000
    is_changed = False

    goaldx = last_dx #defaults to the last dx and dy
    goaldy = last_dy

    for x1, y1, x2, y2 in rects: #Draws the rectangles detected earlier
        cv2.rectangle(img, (x1, y1), (x2, y2), (127, 255, 0), 2)
        dx = (x1 + x2) * 0.5 - x_mid
        dy = (y1 + y2) * 0.5 - y_mid
        is_changed = True

        dist = ((dx - last_dx)**2 + (dy-last_dy)**2)**0.5

        if(dist < closest):
            closest = dist
            goaldx = dx
            goaldy = dy
            last_dx = goaldx
            last_dy = goaldy

    if is_changed:
        if serial_on:
            ser.write(str(int(dx)) + ";" + str(int(dy)) + ";")
        print("dx: " + str(int(dx)) + " dy: " + str(int(dy)) + " written to serial port")
        time.sleep(0.1)
    #print (dx, dy) #Prints relative position
    cv2.cv.ShowImage("YOUR FACE", cv2.cv.fromarray(img))


#Generates Window, all future images displayed appear on this window
cv2.namedWindow("YOUR FACE", (cv2.CV_WINDOW_AUTOSIZE))

#Creates Camera and establishes the midpoints for calculations
cam = cv2.cv.CaptureFromCAM(1)
cv2.cv.SetCaptureProperty(cam, cv2.cv.CV_CAP_PROP_FRAME_WIDTH, 240)
cv2.cv.SetCaptureProperty(cam, cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 180)
#cam.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, 240)
x_mid = cv2.cv.GetCaptureProperty(cam, cv2.cv.CV_CAP_PROP_FRAME_WIDTH) // 2
y_mid = cv2.cv.GetCaptureProperty(cam, cv2.cv.CV_CAP_PROP_FRAME_HEIGHT) // 2

last_dx = 0
last_dy = 0

if(serial_on):
    ser = serial.Serial(serial_name, 9600)

while True:
    pic = cv2.cv.QueryFrame(cam)
    rects, img = detect(pic)
    box(rects, img)
    cv2.cv.WaitKey(5) #Necessary to have every time you display an image

#Window Cleanup
cv2.cv.destroyWindow("YOUR FACE")
