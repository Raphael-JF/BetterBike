let map = null;
let marker = null;
let lat = null;
let lon = null;

const WAYPOINTS_STORAGE_KEY = "betterbike_waypoints_v1";

// Calibrate toggle (UI + placeholder hooks)
let calibrating = false;

function qs(sel) {
    return document.querySelector(sel);
}

function showToast(message, kind = "ok", timeoutMs = 2200) {
    const host = qs("#toastHost");
    if (!host) return;

    const el = document.createElement("div");
    el.className = `toast ${kind === "error" ? "toastError" : "toastOk"}`;
    el.textContent = message;

    host.appendChild(el);

    window.setTimeout(() => {
        el.remove();
    }, timeoutMs);
}

function setStatusLine() {
    const bleStatus = bleIsConnected() ? "Connected." : "Not connected.";
    const wpStatus = lat !== null && lon !== null
        ? `Waypoint: ${lat.toFixed(6)}, ${lon.toFixed(6)}.`
        : "No waypoint selected.";
    const calStatus = calibrating ? "Calibrating." : "";

    const el = qs("#statusLine");
    if (el) el.textContent = [bleStatus, wpStatus, calStatus].filter(Boolean).join(" ");
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
        await bleConnect();
        showToast("BLE connected.", "ok");
        setStatusLine();
    } catch (e) {
        showToast("BLE error: " + e, "error", 3500);
        setStatusLine();
    }
}

async function sendWaypoint() {
    if (lat === null || lon === null) {
        showToast("Select a waypoint on the map first.", "error");
        return;
    }

    const buffer = new ArrayBuffer(17);
    const view = new DataView(buffer);

    view.setUint8(0, TYPE_WAYPOINT);

    // little endian = true pour ESP32
    view.setFloat64(1, lat, true);
    view.setFloat64(9, lon, true);

    await bleSendFrame(buffer);
    showToast("Waypoint sent.", "ok");
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
        showToast("Invalid choice.", "error");
        return null;
    }

    return selectedIndex;
}

function saveWaypoint() {
    if (lat === null || lon === null) {
        showToast("Select a point on the map before saving.", "error");
        return;
    }

    const defaultName = `wp_${new Date().toISOString().slice(0, 19).replace(/[:T]/g, "-")}`;
    const inputName = prompt("Waypoint name?", defaultName);

    if (inputName === null) {
        return;
    }

    const name = inputName.trim();
    if (!name) {
        showToast("Invalid name.", "error");
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
    showToast(`Saved "${name}".`, "ok");
}

function loadWaypoint() {
    const waypoints = getStoredWaypoints();

    if (waypoints.length === 0) {
        showToast("No saved waypoints.", "error");
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
    showToast(`Loaded "${selected.name}".`, "ok");
}

function renameWaypoint() {
    const waypoints = getStoredWaypoints();

    if (waypoints.length === 0) {
        showToast("No saved waypoints.", "error");
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
        showToast("Invalid name.", "error");
        return;
    }

    if (waypoints.some((wp, index) => index !== selectedIndex && wp.name === newName)) {
        showToast("That name already exists.", "error");
        return;
    }

    waypoints[selectedIndex] = {
        ...current,
        name: newName,
        updatedAt: Date.now()
    };

    setStoredWaypoints(waypoints);
    showToast(`Renamed to "${newName}".`, "ok");
}

function deleteWaypoint() {
    const waypoints = getStoredWaypoints();

    if (waypoints.length === 0) {
        showToast("No saved waypoints.", "error");
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
    showToast(`Deleted "${selected.name}".`, "ok");
}

function centerOnMarker() {
    if (!map || lat === null || lon === null) {
        showToast("No waypoint to center on.", "error");
        return;
    }
    map.setView([lat, lon], map.getZoom());
}

async function startCalibration() {
    const buffer = new ArrayBuffer(1);
    const view = new DataView(buffer);

    view.setUint8(0, TYPE_CALIBRATION);

    bleSendFrame(buffer);
    showToast("Calibration started.", "ok");
}

function stopSaveCalibration() {
    const buffer = new ArrayBuffer(1);
    const view = new DataView(buffer);

    view.setUint8(0, TYPE_STOP_SAVE_CALIBRATION);
}

function updateCalibrationButtonUi() {
    const btn = qs("#btnToggleCalibrate");
    const title = qs("#calibrateBtnTitle");
    const desc = qs("#calibrateBtnDesc");

    if (!btn || !title || !desc) return;

    btn.setAttribute("aria-pressed", calibrating ? "true" : "false");
    title.textContent = calibrating ? "Stop" : "Calibrate";
    desc.textContent = calibrating ? "Calibration running — click to stop" : "Start/stop calibration mode";
}

function toggleCalibration() {
    calibrating = !calibrating;

    if (calibrating) {
        startCalibration();
        showToast("Calibration started.", "ok");
    } else {
        stopSaveCalibration();
        showToast("Calibration stopped.", "ok");
    }

    updateCalibrationButtonUi();
    setStatusLine();
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
    qs("#btnToggleCalibrate").addEventListener("click", toggleCalibration);

    // Map actions 
    qs("#btnSendWaypoint").addEventListener("click", sendWaypoint);
    qs("#btnSaveWaypoint").addEventListener("click", saveWaypoint);
    qs("#btnLoadWaypoint").addEventListener("click", loadWaypoint);
    qs("#btnRenameWaypoint").addEventListener("click", renameWaypoint);
    qs("#btnDeleteWaypoint").addEventListener("click", deleteWaypoint);
    btnCenterMarker.addEventListener("click", centerOnMarker);

    // Start on menu
    updateCalibrationButtonUi();
    showView("#menuView");
    setStatusLine();
}

document.addEventListener("DOMContentLoaded", initUi);
