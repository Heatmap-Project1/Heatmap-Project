import cv2 as cv
import numpy as np
import pandas as pd
from PIL import Image
import rotatee

def rotaflip(x):
    k = Image.open(x)
    krotaflip = k.rotate(180).transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    krotaflip.save(x)

#? defining dnow, dpi with normal column names
dnow = pd.read_csv('finalweather34k.csv')
kL = list(dnow.T.index)
kL[1:] = [i.split('.')[-1] for i in kL[1:]]
a = dnow.columns
b = kL
c = zip(a,b)
dlol = dict(c)
dnow.rename(columns = dlol, inplace = True)

#? defining vars for lat-long to pixel convert
dpi = dnow[['lat','lon']]
west = -180
east = 180
north = 90
south = -90
lat, long = dpi['lat'], dpi['lon']
w,h = 1280, 641
img = np.zeros((h,w,3), np.uint8)
hm = dpi['temp_c'] = dnow['temp_c']
M,m=dpi['temp_c'].max(),dpi['temp_c'].min()

imE = cv.imread('newearth.png')

#? lat-lon to pixel conversion
#dpi['px'], dpi['py'] = np.round(((long-west)/(east-west))*w).astype(int), np.round(((north-lat)/(north-south))*h).astype(int)
dpi["px"] = np.round(((long - west) / (east - west)) * (w - 1)).astype(int)
dpi["py"] = np.round(((north - lat) / (north - south)) * (h - 1)).astype(int)
dpi['tnorm'] = dpi['temp_c'].apply(lambda x: int(np.round(((x-m)/(M-m))*255)))

dpi["px"] = np.round(((long - west) / (east - west)) * (w - 1)).astype(int)
dpi["py"] = np.round(((north - lat) / (north - south)) * (h - 1)).astype(int)

#! redundant stuff; earlier it was plotting just points n applying gaussblur
img[dpi["py"], dpi["px"], 2] =  dpi['tnorm'] # 2 indicates r in bgr channel

blu = cv.GaussianBlur(img, (7,7), cv.BORDER_DEFAULT)



#rotatin n flippen image recieved from 3dim.py
rotaflip('contour.png')

imCG = cv.imread('contour.png')


#? blending like in pillow
imdn = cv.addWeighted(imCG, 0.8, imE, 0.2, 0)

'''print(np.max(np.array(dpi['px'])),np.max(np.array(dpi['py'])))'''
print(dpi, dpi['tnorm'])

cv.imshow('image', imdn)
cv.imwrite('worldnew.png', imdn)
cv.waitKey(0)
