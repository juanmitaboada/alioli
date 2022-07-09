import matplotlib.pyplot as plt
import numpy as np
from numpy import arange
from numpy.random import rand

# fig = plt.figure(figsize=(1,5), dpi=100)
fig = plt.figure()

xmin, xmax = xlim = 0, 1
ymin, ymax = ylim = 0, -50
ax = fig.add_subplot(111, xlim=xlim, ylim=ylim, autoscale_on=False)
X = [[.6, .6], [.7, .7]]

ax.imshow(X, interpolation='bicubic', cmap=plt.cm.afmhot, extent=(xmin, xmax, ymin, ymax), alpha=1)

# -1 is from -30 1 more
ax.bar(0.5, height=-1, width=0.8, bottom=-30, alpha=1, color=['white'])

ax.set_aspect('auto')
plt.ylim((-50, 0))
plt.show()



#import numpy as np
#import matplotlib.pyplot as plt
#
#fig, ax = plt.subplots()
#
#bar = ax.bar([1,2,3,4,5,6],[4,5,6,3,7,5])
#
#def gradientbars(bars):
#    grad = np.atleast_2d(np.linspace(4,1,256)).T
#    ax = bars[0].axes
#    lim = ax.get_xlim()+ax.get_ylim()
#    for bar in bars:
#        bar.set_zorder(1)
#        bar.set_facecolor("none")
#        x,y = bar.get_xy()
#        w, h = bar.get_width(), bar.get_height()
#        ax.imshow(grad, extent=[x,x+w,y,y+h], aspect="auto", zorder=0)
#    ax.axis(lim)
#
#gradientbars(bar)
#
#plt.show() 
