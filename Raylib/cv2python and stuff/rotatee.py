from PIL import Image
import cv2 as cv

def rota(x):
    L = Image.open(x)
    v = L.transpose(Image.ROTATE_90)
    v.save(x)

rota('worldnew.png')