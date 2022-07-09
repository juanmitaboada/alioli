import matplotlib.pyplot as plt
import numpy as np

# fig = plt.figure(figsize=(5,5), dpi=100)
fig = plt.figure(dpi=100)
ax = fig.add_subplot(111, polar=True)
rads = -28 * np.pi / 180
ax.bar(rads, height=60, width=np.pi/50, bottom=0, alpha=0.5, color=['red'])
rads = 132 * np.pi / 180
ax.bar(rads, height=80, width=np.pi/50, bottom=0, alpha=0.5, color=['blue'])

ax.set_theta_zero_location("N")
# ax.set_thetamin(-90)
# ax.set_thetamax(90)

plt.ylim((0, 100))
plt.show()
