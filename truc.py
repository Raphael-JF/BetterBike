from matplotlib import pyplot as plt
from matplotlib.widgets import Slider
import numpy as np

# initial parameters
Rw0 = 0.33   # wheel radius (m)
Of0 = 0.04   # fork offset (m)
Ah0 = 20     # head angle in degrees

def chasse_velo(Rw, Of, Ah_deg):
    Ah = np.deg2rad(Ah_deg)
    return (Rw * np.cos(Ah) - Of) / np.sin(Ah)

fig, ax = plt.subplots()
plt.subplots_adjust(bottom=0.35)

# We'll draw the segments in the update function
ax.set_xlim(-0.1, 0.6)
ax.set_ylim(-0.1, 0.6)
ax.set_aspect('equal')
ax.set_axis_off()

text = ax.text(0.05, 0.95, "", fontsize=12, va='top', ha='left', transform=ax.transAxes)

# Placeholders for the lines
segment_lines = [ax.plot([], [], 'b-', lw=2)[0] for _ in range(3)]

def update(val):
    Ah_deg = slider_Ah.val
    Rw = slider_Rw.val
    Of = slider_Of.val

    Ah = np.deg2rad(Ah_deg)
    zv = chasse_velo(Rw, Of, Ah_deg)

    # Segment 1: axis of the head tube (from (0,0) to intersection with ground)
    # Let's draw it from (0,0) to (x1, y1) where y1 = 0.5 (arbitrary), x1 = y1 / tan(Ah)
    y1 = 0.5
    x1 = y1 / np.tan(Ah)
    segment_lines[0].set_data([0, x1], [0, y1])

    # Segment 2: wheel contact point (vertical from (x1,0) to (x1, Rw))
    segment_lines[1].set_data([x1, x1], [0, Rw])

    # Segment 3: offset (horizontal from (x1,0) to (x1 + Of, 0))
    segment_lines[2].set_data([x1, x1 + Of], [0, 0])

    text.set_text(f"Chasse vélo: {zv:.3f} m\nAngle de direction: {Ah_deg:.1f}°")
    fig.canvas.draw_idle()

ax_Ah = plt.axes([0.2, 0.25, 0.65, 0.03])
ax_Rw = plt.axes([0.2, 0.15, 0.65, 0.03])
ax_Of = plt.axes([0.2, 0.10, 0.65, 0.03])

slider_Ah = Slider(ax_Ah, 'Ah (°)', 5, 85, valinit=Ah0)
slider_Rw = Slider(ax_Rw, 'Rw (m)', 0.1, 0.7, valinit=Rw0)
slider_Of = Slider(ax_Of, 'Of (m)', 0.0, 0.2, valinit=Of0)

slider_Ah.on_changed(update)
slider_Rw.on_changed(update)
slider_Of.on_changed(update)

update(None)

plt.show()
