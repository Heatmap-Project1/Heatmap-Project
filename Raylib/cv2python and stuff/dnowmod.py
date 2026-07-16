import pandas as pd
import numpy as np

dnow = pd.read_csv('finalweather34k.csv')
kL = list(dnow.T.index)
kL[1:] = [i.split('.')[-1] for i in kL[1:]]
a = dnow.columns
b = kL
c = zip(a,b)
dlol = dict(c)
dnow.rename(columns = dlol, inplace = True)

dpi = dnow[['lat','lon']]
west = -180
east = 180
north = 90
south = -90
lat, long = dpi['lat'], dpi['lon']
w,h = 1280, 641
img = np.zeros((h,w,3), np.uint8)
hm = dpi['temp_c'] = dnow['temp_c']
dpi['heat_index_c'] = dnow['heatindex_c']



#dpi['px'], dpi['py'] = np.round(((long-west)/(east-west))*w).astype(int), np.round(((north-lat)/(north-south))*h).astype(int)
dpi["px"] = np.round(((long - west) / (east - west)) * (w - 1)).astype(int)
dpi["py"] = np.round(((north - lat) / (north - south)) * (h - 1)).astype(int)
#dpi['tnorm'] = dpi['temp_c'].apply(lambda x: int(np.round(((x-m)/(M-m))*255)))

dpi["px"] = np.round(((long - west) / (east - west)) * (w - 1)).astype(int)
dpi["py"] = np.round(((north - lat) / (north - south)) * (h - 1)).astype(int)