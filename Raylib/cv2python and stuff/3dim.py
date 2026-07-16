import matplotlib.pyplot as plt
from dnowmod import dnow, dpi
import numpy as np
from scipy.ndimage import gaussian_filter



def cRGBA(x):
    c = tuple()
    for i in x:
        c+= (int(np.round(i*255)),)
    return c

turbo = plt.get_cmap('turbo')
tmp = dpi['temp_c']
Mt,mt = max(tmp), min(tmp)

hi = dpi['heat_index_c']
Mh,mh = max(hi), min(hi)

dpi['normal_T'] = (tmp - mt)/(Mt - mt)
dpi['normal_HI'] = (hi - mh)/(Mh - mh)

x_ax = np.arange(0,1280)
y_ax = np.arange(0,641)
x1,y1 = np.meshgrid(x_ax,y_ax)

z = np.zeros((1280,641))
z[dpi['px'], dpi['py']] = dpi['normal_HI']
z = z.T
Z_smooth = gaussian_filter(z, sigma=1)

print(dpi, dnow.columns)

d = 100

fig = plt.figure(figsize=(1280/d, 641/d), dpi=d)
ax = fig.add_axes([0, 0, 1, 1])   # axes fill the whole figure
ax.contourf(x1,y1,Z_smooth,cmap = 'turbo')
ax.set_axis_off()
plt.savefig(
    "contour.png",
    bbox_inches="tight",
    pad_inches=0
)
plt.show()

print(dpi, max(dpi['px']), max(dpi['py']))