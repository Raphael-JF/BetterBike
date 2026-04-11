import numpy as np
import matplotlib.pyplot as plt

# fichiers
fichier = "data.txt"

# listes
temps = []
X = []
Y = []

with open(fichier, "r") as f:
    for ligne in f:
        # nettoyer la ligne
        ligne = ligne.strip()
        if not ligne:
            continue

        # split
        parties = ligne.split("|")

        t = float(parties[0].split(":")[1])
        x = float(parties[1].split(":")[1])
        y = float(parties[2].split(":")[1])

        temps.append(t)
        X.append(x)
        Y.append(y)

# plot
plt.figure()
plt.scatter(X, Y)
plt.scatter(np.mean(X), np.mean(Y), color='red', label='Mean')
plt.xlabel("X")
plt.ylabel("Y")
plt.title("Calibration magnétomètre (vue XY)")
# draw 0 axes
plt.axhline(0, color='gray', lw=0.5)
plt.axvline(0, color='gray', lw=0.5)

plt.axis("equal")  # important !
plt.show()