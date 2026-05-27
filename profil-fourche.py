from matplotlib import pyplot as plt
from matplotlib.widgets import Slider
from matplotlib.patches import Circle
import numpy as np

Rw0 = 0.350   # rayon roue 700c (m)
Of0 = 0.045   # offset fourche route typique (m)
Ah0 = 72.0    # angle de tête en degrés

def chasse_velo(Rw, Of, Ah_deg):
    Ah = np.deg2rad(Ah_deg)
    return (Rw * np.cos(Ah) - Of) / np.sin(Ah)

fig, ax = plt.subplots(figsize=(8, 8))
plt.subplots_adjust(bottom=0.28)

ax.set_aspect('equal')
ax.set_axis_off()
ax.set_title("Géométrie de fourche — chasse vélo", fontsize=12, pad=10)

wheel_patch = Circle((0, 0), 0.35, fill=False, edgecolor='#1D9E75', linewidth=2, zorder=2)
ax.add_patch(wheel_patch)

line_axis,   = ax.plot([], [], color='#3B8BD4', lw=2,   label='Axe de direction', zorder=3)
line_offset, = ax.plot([], [], color='#D85A30', lw=2.5, label='Offset fourche',   zorder=4)
line_trail,  = ax.plot([], [], color='#533AB7', lw=2,   linestyle='--',
                        label='Chasse (trail)', zorder=3)

dot_contact = ax.plot([], [], 'o', color='#D85A30', ms=7,  zorder=5)[0]
dot_center  = ax.plot([], [], 'o', color='#1D9E75', ms=5,  zorder=5)[0]
dot_axisgnd = ax.plot([], [], 'o', color='#3B8BD4', ms=5,  zorder=5)[0]
ground_line, = ax.plot([], [], color='#888', lw=1.2, zorder=0)

text_info = ax.text(0.02, 0.97, "", fontsize=11, va='top', ha='left',
                    transform=ax.transAxes, family='monospace')

ax.legend(loc='upper right', fontsize=9, framealpha=0.6)

def update(val):
    Ah_deg = slider_Ah.val
    Rw     = slider_Rw.val / 1000.0
    Of     = slider_Of.val / 1000.0
    Ah     = np.deg2rad(Ah_deg)

    #zv = chasse_velo(Rw, Of, Ah_deg)

    # Contact roue/sol (fixe)
    cx = 0.0
    cy = 0.0

    # Centre de la roue
    wcx, wcy = cx, Rw

    # Vecteur directeur de l'axe de direction :
    # l'axe est incliné de Ah par rapport à la verticale,
    # il pointe vers le bas-arrière → direction (-sin Ah, -cos Ah)
    dx = -np.sin(Ah)
    dy = -np.cos(Ah)

    # Vecteur perpendiculaire à l'axe (vers l'avant = +x)
    # Rotation de 90° de (dx,dy) dans le sens trigo : (-dy, dx) = (cos Ah, -sin Ah)
    perp_x =  np.cos(Ah)
    perp_y = -np.sin(Ah)

    # Point d'attache fourche sur l'axe = centre roue + offset * perp
    fp_x = wcx + Of * perp_x
    fp_y = wcy + Of * perp_y

    # Intersection axe/sol : fp + t*(dx,dy) avec y=0
    # fp_y + t*dy = 0  →  t = -fp_y / dy
    t_gnd = -fp_y / dy
    ag_x  = fp_x + dx * t_gnd   # pied de l'axe au sol
    zv = abs(ag_x)
    ag_y  = 0.0

    # Prolongation de l'axe vers le haut (t négatif = sens opposé)
    t_top = -Rw * 1.8 / np.cos(Ah)   # remonte d'environ 1.8*Rw
    at_x  = fp_x + dx * t_top
    at_y  = fp_y + dy * t_top

    trail_y = -0.02

    # Sol adapté à l'étendue horizontale
    x_min = min(ag_x, cx) - 0.08
    x_max = max(ag_x, cx, at_x) + 0.08
    ground_line.set_data([x_min, x_max], [0, 0])

    line_axis.set_data(  [ag_x, at_x], [ag_y, at_y])
    line_offset.set_data([wcx,  fp_x], [wcy,  fp_y])
    line_trail.set_data( [ag_x, cx],   [trail_y, trail_y])

    wheel_patch.center = (wcx, wcy)
    wheel_patch.radius = Rw

    dot_contact.set_data([cx],  [0])
    dot_center.set_data( [wcx], [wcy])
    dot_axisgnd.set_data([ag_x],[0])

    text_info.set_text(
        f"Ah    = {Ah_deg:.1f}°\n"
        f"Rw    = {Rw*1000:.0f} mm\n"
        f"Of    = {Of*1000:.1f} mm\n"
        f"──────────────\n"
        f"Trail = {zv*1000:.1f} mm"
    )

    # Recalcul automatique des limites
    margin = 0.06
    all_x = [ag_x, at_x, cx, wcx - Rw, wcx + Rw]
    all_y = [0,    at_y, 0,  0,          wcy + Rw]
    ax.set_xlim(min(all_x) - margin, max(all_x) + margin)
    ax.set_ylim(min(all_y) - margin * 2, max(all_y) + margin)

    fig.canvas.draw_idle()

ax_Ah = plt.axes([0.18, 0.19, 0.65, 0.03])
ax_Rw = plt.axes([0.18, 0.13, 0.65, 0.03])
ax_Of = plt.axes([0.18, 0.07, 0.65, 0.03])

slider_Ah = Slider(ax_Ah, 'Ah (°)',    5,  85,  valinit=Ah0,       valstep=0.5)
slider_Rw = Slider(ax_Rw, 'Rw (mm)', 200, 400,  valinit=Rw0*1000,  valstep=1)
slider_Of = Slider(ax_Of, 'Of (mm)',   0, 100,   valinit=Of0*1000,  valstep=0.5)

slider_Ah.on_changed(update)
slider_Rw.on_changed(update)
slider_Of.on_changed(update)

update(None)
plt.show()
