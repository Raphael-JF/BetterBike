let map = null;
let marker = null;
let lat = null;
let lon = null;

const WAYPOINTS_STORAGE_KEY = "betterbike_waypoints_v1";

// BLE
let device = null;
let characteristic = null;

function qs(sel) {
    return document.querySelector(sel);
}

function setStatusLine() {
    const bleStatus = characteristic ? "Connected." : "Not connected.";
    const wpStatus = lat !== null && lon !== null
        ? `Waypoint selected: ${lat.toFixed(6)}, ${lon.toFixed(6)}.`
        : "No waypoint selected.";

    const el = qs("#statusLine");
    if (el) el.textContent = `${bleStatus} ${wpStatus}`;
}

function showView(viewId) {
    const menuView = qs("#menuView");
    const mapView = qs("#mapView");

    menuView.classList.remove("viewActive");
    mapView.classList.remove("viewActive");

    qs(viewId).classList.add("viewActive");
}

function ensureMapInitialized() {
    if (map) return;

    map = L.map('map').setView([44.84, -0.58], 13); // default: Bordeaux

    // OpenStreetMap tiles
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        maxZoom: 19
    }).addTo(map);

    // Click on map
    map.on('click', function (e) {
        lat = e.latlng.lat;
        lon = e.latlng.lng;

        qs("#lat").innerText = lat.toFixed(6);
        qs("#lon").innerText = lon.toFixed(6);

        if (marker) {
            marker.setLatLng(e.latlng);
        } else {
            marker = L.marker(e.latlng).addTo(map);
        }

        setStatusLine();
    });

    // After being shown, Leaflet sometimes needs a layout refresh
    setTimeout(() => map.invalidateSize(), 0);
}

async function connectBLE() {
    try {
        device = await navigator.bluetooth.requestDevice({
            acceptAllDevices: true
        });

        const server = await device.gatt.connect();

        // get all services
        const services = await server.getPrimaryServices();

        for (const service of services) {
            try {
                const characteristics = await service.getCharacteristics();

                for (const char of characteristics) {
                    // first writable
                    if (char.properties.write || char.properties.writeWithoutResponse) {
                        characteristic = char;
                        alert("Connected and writable characteristic found!");
                        setStatusLine();
                        return;
                    }
                }
            } catch (e) {
                // ignore and continue scanning other services
            }
        }

        alert("No writable characteristic found.");
        setStatusLine();
    } catch (e) {
        alert("Error: " + e);
        setStatusLine();
    }
}

async function sendWaypoint() {
    if (!characteristic || lat === null || lon === null) {
        alert("Not connected or no waypoint selected.");
        return;
    }

    const msg = `${lat};${lon}\n`;
    const encoder = new TextEncoder();
    await characteristic.writeValue(encoder.encode(msg));

    alert("Sent: " + msg);
}

function getStoredWaypoints() {
    try {
        const raw = localStorage.getItem(WAYPOINTS_STORAGE_KEY);
        if (!raw) return [];
        const parsed = JSON.parse(raw);
        return Array.isArray(parsed) ? parsed : [];
    } catch (e) {
        return [];
    }
}

function setStoredWaypoints(waypoints) {
    localStorage.setItem(WAYPOINTS_STORAGE_KEY, JSON.stringify(waypoints));
}

function formatWaypointList(waypoints) {
    return waypoints
        .map((wp, index) => `${index + 1}. ${wp.name} (${wp.lat.toFixed(6)}, ${wp.lon.toFixed(6)})`)
        .join("\n");
}

function promptWaypointIndex(waypoints, actionLabel) {
    const list = formatWaypointList(waypoints);
    const choice = prompt(`${actionLabel} (number):\n${list}`);

    if (choice === null) {
        return null;
    }

    const selectedIndex = Number.parseInt(choice, 10) - 1;
    if (!Number.isInteger(selectedIndex) || selectedIndex < 0 || selectedIndex >= waypoints.length) {
        alert("Invalid choice.");
        return null;
    }

    return selectedIndex;
}

function saveWaypoint() {
    if (lat === null || lon === null) {
        alert("Select a point on the map before saving.");
        return;
    }

    const defaultName = `wp_${new Date().toISOString().slice(0, 19).replace(/[:T]/g, "-")}`;
    const inputName = prompt("Waypoint name?", defaultName);

    if (inputName === null) {
        return;
    }

    const name = inputName.trim();
    if (!name) {
        alert("Invalid name.");
        return;
    }

    const waypoints = getStoredWaypoints();
    const existingIndex = waypoints.findIndex(wp => wp.name === name);
    const data = { name, lat, lon, updatedAt: Date.now() };

    if (existingIndex >= 0) {
        waypoints[existingIndex] = data;
    } else {
        waypoints.push(data);
    }

    setStoredWaypoints(waypoints);
    alert(`Waypoint "${name}" saved.`);
}

function loadWaypoint() {
    const waypoints = getStoredWaypoints();

    if (waypoints.length === 0) {
        alert("No saved waypoints.");
        return;
    }

    const selectedIndex = promptWaypointIndex(waypoints, "Choose a waypoint");
    if (selectedIndex === null) {
        return;
    }

    const selected = waypoints[selectedIndex];
    lat = selected.lat;
    lon = selected.lon;

    qs("#lat").innerText = lat.toFixed(6);
    qs("#lon").innerText = lon.toFixed(6);

    const latlng = L.latLng(lat, lon);

    ensureMapInitialized();

    if (marker) {
        marker.setLatLng(latlng);
    } else {
        marker = L.marker(latlng).addTo(map);
    }

    map.setView(latlng, map.getZoom());
    showView("#mapView");
    setTimeout(() => map.invalidateSize(), 0);

    setStatusLine();
    alert(`Waypoint "${selected.name}" loaded.`);
}

function renameWaypoint() {
    const waypoints = getStoredWaypoints();

    if (waypoints.length === 0) {
        alert("No saved waypoints.");
        return;
    }

    const selectedIndex = promptWaypointIndex(waypoints, "Waypoint to rename");
    if (selectedIndex === null) {
        return;
    }

    const current = waypoints[selectedIndex];
    const inputName = prompt("New name?", current.name);

    if (inputName === null) {
        return;
    }

    const newName = inputName.trim();
    if (!newName) {
        alert("Invalid name.");
        return;
    }

    if (waypoints.some((wp, index) => index !== selectedIndex && wp.name === newName)) {
        alert("That name already exists.");
        return;
    }

    waypoints[selectedIndex] = {
        ...current,
        name: newName,
        updatedAt: Date.now()
    };

    setStoredWaypoints(waypoints);
    alert(`Waypoint "${current.name}" renamed to "${newName}".`);
}

function deleteWaypoint() {
    const waypoints = getStoredWaypoints();

    if (waypoints.length === 0) {
        alert("No saved waypoints.");
        return;
    }

    const selectedIndex = promptWaypointIndex(waypoints, "Waypoint to delete");
    if (selectedIndex === null) {
        return;
    }

    const selected = waypoints[selectedIndex];
    const confirmed = confirm(`Delete "${selected.name}"?`);

    if (!confirmed) {
        return;
    }

    waypoints.splice(selectedIndex, 1);
    setStoredWaypoints(waypoints);
    alert(`Waypoint "${selected.name}" deleted.`);
}

function centerOnMarker() {
    if (!map || lat === null || lon === null) return;
    map.setView([lat, lon], map.getZoom());
}

function initUi() {
    const btnGoMenu = qs("#btnGoMenu");
    const btnBackToMenu = qs("#btnBackToMenu");
    const btnOpenMap = qs("#btnOpenMap");
    const btnCenterMarker = qs("#btnCenterMarker");

    btnGoMenu.addEventListener("click", () => {
        showView("#menuView");
        setStatusLine();
    });

    btnBackToMenu.addEventListener("click", () => {
        showView("#menuView");
        setStatusLine();
    });

    btnOpenMap.addEventListener("click", () => {
        ensureMapInitialized();
        showView("#mapView");
        setTimeout(() => map.invalidateSize(), 0);
        setStatusLine();
    });

    qs("#btnConnectBle").addEventListener("click", connectBLE);
    qs("#btnSendWaypoint").addEventListener("click", sendWaypoint);
    qs("#btnSaveWaypoint").addEventListener("click", saveWaypoint);
    qs("#btnLoadWaypoint").addEventListener("click", loadWaypoint);
    qs("#btnRenameWaypoint").addEventListener("click", renameWaypoint);
    qs("#btnDeleteWaypoint").addEventListener("click", deleteWaypoint);

    btnCenterMarker.addEventListener("click", centerOnMarker);

    // Start on menu
    showView("#menuView");
    setStatusLine();
}

document.addEventListener("DOMContentLoaded", initUi);
