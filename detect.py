'''
TEST FILE:
Uses haarcascade's XML file to train CascadeClassifier to
  recognize faces.
Infinitely runs through video input and detects faces.
  Boxes the face and displays at relatively live speed.

--Mitchell
'''

import cv2
import numpy as np

''' - Jas '''

#Imports the training data.
cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")

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
    for x1, y1, x2, y2 in rects: #Draws the rectangles detected earlier
        cv2.rectangle(img, (x1, y1), (x2, y2), (127, 255, 0), 2)
        dx = (x1 + x2) * 0.5 - x_mid
        dy = (y1 + y2) * 0.5 - y_mid
        print (dx, dy) #Prints relative position
    cv2.cv.ShowImage("YOUR FACE", cv2.cv.fromarray(img))


#Generates Window, all future images displayed appear on this window
cv2.namedWindow("YOUR FACE", (cv2.CV_WINDOW_AUTOSIZE))

#Creates Camera and establishes the midpoints for calculations
cam = cv2.cv.CaptureFromCAM(0)
x_mid = cv2.cv.GetCaptureProperty(cam, cv2.cv.CV_CAP_PROP_FRAME_WIDTH) // 2
y_mid = cv2.cv.GetCaptureProperty(cam, cv2.cv.CV_CAP_PROP_FRAME_HEIGHT) // 2

while True:
    pic = cv2.cv.QueryFrame(cam)
    rects, img = detect(pic)
    box(rects, img)
    cv2.cv.WaitKey(5) #Necessary to have every time you display an image

#Window Cleanup
cv2.cv.destroyWindow("YOUR FACE")