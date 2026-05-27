from matplotlib import pyplot as plt
from matplotlib.widgets import Slider
import numpy as np

# initial parameters
Rw0 = 0.33   # wheel radius (m)
Of0 = 0.04   # fork offset (m)
Ah0 = np.deg2rad(20)
Ar0 = np.deg2rad(25)

def chasse_velo(Rw, Of, Ah):
    return (Rw*np.cos(Ah) - Of) / np.sin(Ah)

def chasse_moto(Rw, Of, Ar):
    return (Rw*np.sin(Ar) - Of) / np.cos(Ar)

fig, ax = plt.subplots()
plt.subplots_adjust(bottom=0.35)

text = ax.text(0.1, 0.5, "", fontsize=12)
ax.set_axis_off()

def update(val):
    Ah = slider_Ah.val
    Ar = slider_Ar.val
    Rw = slider_Rw.val
    Of = slider_Of.val
    
    zv = chasse_velo(Rw, Of, Ah)
    zm = chasse_moto(Rw, Of, Ar)
    
    text.set_text(f"Chasse vélo: {zv:.3f} m\nChasse moto: {zm:.3f} m")
    fig.canvas.draw_idle()

ax_Ah = plt.axes([0.2, 0.25, 0.65, 0.03])
ax_Ar = plt.axes([0.2, 0.20, 0.65, 0.03])
ax_Rw = plt.axes([0.2, 0.15, 0.65, 0.03])
ax_Of = plt.axes([0.2, 0.10, 0.65, 0.03])

slider_Ah = Slider(ax_Ah, 'Ah (rad)', 0.1, 1.2, valinit=Ah0)
slider_Ar = Slider(ax_Ar, 'Ar (rad)', 0.1, 1.2, valinit=Ar0)
slider_Rw = Slider(ax_Rw, 'Rw', 0.1, 0.7, valinit=Rw0)
slider_Of = Slider(ax_Of, 'Of', 0.0, 0.2, valinit=Of0)

slider_Ah.on_changed(update)
slider_Ar.on_changed(update)
slider_Rw.on_changed(update)
slider_Of.on_changed(update)

update(None)

plt.show()
