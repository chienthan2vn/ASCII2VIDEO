import cv2
import numpy as np
from os import listdir

img_array = []
link = './image/'
for filename in listdir(link):
    img = cv2.imread(link + filename)
    height, width, layers = img.shape
    size = (width,height)
    img_array.append(img)
# print(img_array)
out = cv2.VideoWriter('project.avi',cv2.VideoWriter_fourcc(*'MJPG'), 5, size)
 
for i in range(len(img_array)):
    out.write(img_array[i])
out.release()