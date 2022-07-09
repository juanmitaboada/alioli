import matplotlib.pyplot as plt
import numpy as np

fig = plt.figure(figsize=(5,5), dpi=100)
ax = fig.add_subplot(111, polar=True)
rads = 30 * np.pi / 180
ax.bar(rads, height=1, width=np.pi/50, bottom=0, alpha=1, color=['blue'])
rads = 28 * np.pi / 180
ax.bar(rads, height=0.8, width=np.pi/50, bottom=0, alpha=1, color=['red'])

ax.set_thetamin(-90)
ax.set_thetamax(90)

plt.show()
