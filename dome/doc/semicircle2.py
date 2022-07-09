import matplotlib.pyplot as plt
import numpy as np

theta = np.linspace(0,np.pi)
r = np.sin(theta)

fig = plt.figure()
ax = fig.add_subplot(111, polar=True)
c = ax.scatter(theta, r, c=r, s=10, cmap='hsv', alpha=0.75)

ax.set_thetamin(-90)
ax.set_thetamax(90)

plt.show()
