import matplotlib.pyplot as plt

# data
label = ["A", "B", "C"]
val = [1,2,3]

# append data and assign color
label.append("")
val.append(sum(val))  # 50% blank
colors = ['red', 'blue', 'green', 'k']

# plot
plt.figure(figsize=(8,6),dpi=100)

wedges, labels=plt.pie(val, wedgeprops=dict(width=0.4,edgecolor='w'),labels=label, colors=colors)
# I tried this method
wedges[-1].set_visible(False)
plt.show()
