from matplotlib import pyplot as plt
from matplotlib.widgets import Slider
from matplotlib.patches import Circle
import numpy as np

# Paramètres initiaux — roue 700c route (rayon = 350 mm)
Rw0 = 0.350   # rayon roue 700c (m)
Of0 = 0.045   # offset fourche route typique (m)
Ah0 = 72.0    # angle de tête en degrés (route ~72°)

import math

def chasse_velo(Rw, Of, Ah_deg):
    """
    Affiche le schéma de la chasse d'un vélo et calcule la chasse géométrique correcte.
    Rw: Rayon de la roue (mm)
    Of: Offset de la fourche (mm)
    Ah_deg: Angle de chasse (degrés)
    """
    import matplotlib.pyplot as plt

    # Conversion en radians
    Ah = math.radians(Ah_deg)

    # Position du centre de la roue
    x_wheel = 0
    y_wheel = 0

    # Axe de direction (passe par l'axe de rotation de la fourche)
    # On part du haut (au-dessus de la roue) et on descend vers le sol
    # L'axe doit "rejoindre l'intérieur de la roue" donc il coupe le sol à l'intérieur du pneu
    # Calcul du point d'intersection de l'axe de direction avec le sol (y= -Rw)
    # Equation de l'axe: x = y * tan(Ah)
    x_sol = -Rw * math.tan(Ah)
    y_sol = -Rw

    # Offset de la fourche: déplacement perpendiculaire à l'axe de direction
    # Calcul du vecteur perpendiculaire à l'axe de direction
    dx_offset = Of * math.cos(Ah)
    dy_offset = Of * math.sin(Ah)

    # Position de l'axe de roue (après offset)
    x_axis = x_sol + dx_offset
    y_axis = y_sol + dy_offset

    # Calcul de la chasse géométrique (distance horizontale entre l'axe de direction au sol et le point de contact roue-sol)
    chasse = abs(x_sol - x_wheel)

    # Affichage
    fig, ax = plt.subplots()
    # Roue
    circle = plt.Circle((x_wheel, y_wheel), Rw, fill=False, color='black', linewidth=2)
    ax.add_patch(circle)
    # Axe de direction
    ax.plot([x_sol, 0], [y_sol, 0], color='red', label="Axe de direction")
    # Axe de roue (après offset)
    ax.plot([x_axis, x_axis], [y_axis, y_axis + 2*Rw], color='blue', label="Axe de roue (offset)")
    # Point de contact roue-sol
    ax.plot([x_wheel], [y_sol], 'ko', label="Contact roue-sol")
    # Chasse (trait horizontal)
    ax.plot([x_sol, x_wheel], [y_sol, y_sol], 'g--', label="Chasse (géométrique)")

    ax.set_aspect('equal')
    ax.set_xlabel('x (mm)')
    ax.set_ylabel('y (mm)')
    ax.legend()
    ax.set_title(f"Chasse géométrique = {chasse:.1f} mm")
    ax.grid(True)
    plt.show()
    return chasse
    Ah = np.deg2rad(Ah_deg)
    return (Rw * np.cos(Ah) - Of) / np.sin(Ah)

fig, ax = plt.subplots(figsize=(7, 6))
plt.subplots_adjust(bottom=0.30)

ax.set_xlim(-0.05, 0.70)
ax.set_ylim(-0.08, 0.80)
ax.set_aspect('equal')
ax.set_axis_off()
ax.set_title("Géométrie de fourche — chasse vélo", fontsize=12, pad=10)

# Sol
ax.axhline(0, color='#888', linewidth=1.2, zorder=0)

# Éléments graphiques
line_axis,   = ax.plot([], [], color='#3B8BD4', lw=2,   label='Axe de direction', zorder=3)
line_offset, = ax.plot([], [], color='#D85A30', lw=2.5, label='Offset fourche',   zorder=4)
line_trail,  = ax.plot([], [], color='#533AB7', lw=2,   linestyle='--',
                        label='Chasse (trail)', zorder=3)
wheel_patch  = Circle((0, 0), 0.35, fill=False, edgecolor='#1D9E75', linewidth=2, zorder=2)
ax.add_patch(wheel_patch)

dot_contact = ax.plot([], [], 'o', color='#D85A30', ms=7, zorder=5)[0]
dot_center  = ax.plot([], [], 'o', color='#1D9E75', ms=5, zorder=5)[0]
dot_axisgnd = ax.plot([], [], 'o', color='#3B8BD4', ms=5, zorder=5)[0]

text_info = ax.text(0.02, 0.97, "", fontsize=11, va='top', ha='left',
                    transform=ax.transAxes, family='monospace')

ax.legend(loc='upper right', fontsize=9, framealpha=0.6)

def update(val):
    Ah_deg = slider_Ah.val
    Rw     = slider_Rw.val
    Of     = slider_Of.val
    Ah     = np.deg2rad(Ah_deg)

    zv = chasse_velo(Rw, Of, Ah_deg)

    # --- Géométrie ---
    # Point de contact roue/sol
    cx, cy = 0.25, 0.0         # contact fixe sur le sol

    # Centre de la roue
    wcx, wcy = cx, Rw

    # Offset : perpendiculaire à l'axe de direction
    # Axe de direction : vecteur unitaire (sin Ah, cos Ah) depuis le bas
    # Perpendiculaire (vers l'avant) : (cos Ah, -sin Ah)
    perp_x =  np.cos(Ah)
    perp_y = -np.sin(Ah)

    # Point d'attache de la fourche sur l'axe (depuis centre de roue + offset)
    fp_x = wcx + Of * perp_x
    fp_y = wcy + Of * perp_y

    # L'axe passe par fp dans la direction (sin Ah, cos Ah)
    # Intersection avec le sol (y=0) : y = fp_y + cos(Ah)*t = 0 → t = -fp_y / cos(Ah)
    t_gnd   = -fp_y / np.cos(Ah)
    ag_x    = fp_x + np.sin(Ah) * t_gnd   # intersection axe/sol
    ag_y    = 0.0

    # Prolonger l'axe vers le haut
    t_top   = -0.75
    at_x    = fp_x + np.sin(Ah) * t_top
    at_y    = fp_y + np.cos(Ah) * t_top

    # Chasse au sol (de ag_x à cx, à y légèrement au-dessus du sol)
    trail_y = -0.025

    # --- Mise à jour des tracés ---
    line_axis.set_data([ag_x, at_x], [ag_y, at_y])
    line_offset.set_data([wcx, fp_x], [wcy, fp_y])
    line_trail.set_data([ag_x, cx],  [trail_y, trail_y])

    wheel_patch.center = (wcx, wcy)
    wheel_patch.radius = Rw

    dot_contact.set_data([cx],  [0])
    dot_center.set_data( [wcx], [wcy])
    dot_axisgnd.set_data([ag_x],[0])

    text_info.set_text(
        f"Ah  = {Ah_deg:.1f}°\n"
        f"Rw  = {Rw*1000:.0f} mm\n"
        f"Of  = {Of*1000:.1f} mm\n"
        f"────────────\n"
        f"Trail = {zv*1000:.1f} mm"
    )

    fig.canvas.draw_idle()


# Sliders
ax_Ah = plt.axes([0.18, 0.20, 0.65, 0.03])
ax_Rw = plt.axes([0.18, 0.14, 0.65, 0.03])
ax_Of = plt.axes([0.18, 0.08, 0.65, 0.03])

slider_Ah = Slider(ax_Ah, 'Ah (°)',   5,    85,   valinit=Ah0, valstep=0.5)
slider_Rw = Slider(ax_Rw, 'Rw (mm)', 200,  400,   valinit=Rw0*1000, valstep=1)
slider_Of = Slider(ax_Of, 'Of (mm)',  0,    100,   valinit=Of0*1000, valstep=0.5)

# Les sliders Rw et Of affichent des mm mais on reconvertit en m dans update
_orig_update = update
def update(val):
    slider_Rw.valtext.set_text(f'{slider_Rw.val:.0f}')
    slider_Of.valtext.set_text(f'{slider_Of.val:.1f}')
    # Patch : lire en mm, passer en m
    Ah_deg = slider_Ah.val
    Rw     = slider_Rw.val / 1000.0
    Of     = slider_Of.val / 1000.0
    Ah     = np.deg2rad(Ah_deg)
    zv     = chasse_velo(Rw, Of, Ah_deg)

    cx, cy   = 0.25, 0.0
    wcx, wcy = cx, Rw

    perp_x =  np.cos(Ah)
    perp_y = -np.sin(Ah)
    fp_x = wcx + Of * perp_x
    fp_y = wcy + Of * perp_y

    t_gnd = -fp_y / np.cos(Ah)
    ag_x  = fp_x + np.sin(Ah) * t_gnd
    ag_y  = 0.0

    t_top = -0.75
    at_x  = fp_x + np.sin(Ah) * t_top
    at_y  = fp_y + np.cos(Ah) * t_top

    trail_y = -0.025

    line_axis.set_data(  [ag_x, at_x], [ag_y, at_y])
    line_offset.set_data([wcx,  fp_x], [wcy,  fp_y])
    line_trail.set_data( [ag_x, cx],   [trail_y, trail_y])

    wheel_patch.center = (wcx, wcy)
    wheel_patch.radius = Rw

    dot_contact.set_data([cx],  [0])
    dot_center.set_data( [wcx], [wcy])
    dot_axisgnd.set_data([ag_x],[0])

    text_info.set_text(
        f"Ah  = {Ah_deg:.1f}°\n"
        f"Rw  = {Rw*1000:.0f} mm\n"
        f"Of  = {Of*1000:.1f} mm\n"
        f"────────────\n"
        f"Trail = {zv*1000:.1f} mm"
    )

    fig.canvas.draw_idle()


slider_Ah.on_changed(update)
slider_Rw.on_changed(update)
slider_Of.on_changed(update)

update(None)
plt.show()
