let map = null;
let marker = null;
let lat = null;
let lon = null;

const WAYPOINTS_STORAGE_KEY = "betterbike_waypoints_v1";

// Calibration UI state
let calibrating = false;

// Live calibration plot state
let calPoints = [];
let calMaxPoints = 2000;

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
    const calibrationView = qs("#calibrationView");

    menuView.classList.remove("viewActive");
    mapView.classList.remove("viewActive");
    calibrationView.classList.remove("viewActive");

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

function getCanvasCtx() {
    const canvas = qs("#calibrationCanvas");
    if (!canvas) return null;
    const ctx = canvas.getContext("2d");
    return ctx;
}

function resizeCalibrationCanvasToCssPixels() {
    const canvas = qs("#calibrationCanvas");
    if (!canvas) return;
    const dpr = window.devicePixelRatio || 1;

    const cssW = canvas.clientWidth || 900;
    const cssH = canvas.clientHeight || 520;

    const pxW = Math.max(1, Math.floor(cssW * dpr));
    const pxH = Math.max(1, Math.floor(cssH * dpr));

    if (canvas.width !== pxW || canvas.height !== pxH) {
        canvas.width = pxW;
        canvas.height = pxH;
    }
}

function drawCalibrationScatter() {
    const canvas = qs("#calibrationCanvas");
    const ctx = getCanvasCtx();
    if (!canvas || !ctx) return;

    resizeCalibrationCanvasToCssPixels();

    const w = canvas.width;
    const h = canvas.height;

    ctx.clearRect(0, 0, w, h);

    // Background grid
    ctx.save();
    ctx.globalAlpha = 0.25;
    ctx.strokeStyle = "rgba(255,255,255,0.18)";
    ctx.lineWidth = 1;

    const gridStep = Math.floor(Math.min(w, h) / 10);
    for (let x = 0; x <= w; x += gridStep) {
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, h);
        ctx.stroke();
    }
    for (let y = 0; y <= h; y += gridStep) {
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(w, y);
        ctx.stroke();
    }
    ctx.restore();

    if (calPoints.length === 0) {
        ctx.save();
        ctx.fillStyle = "rgba(255,255,255,0.55)";
        ctx.font = "16px system-ui, sans-serif";
        ctx.fillText("No calibration points yet.", 18, 28);
        ctx.restore();
        return;
    }

    // Scale points to fit, using max abs on x/y
    let maxAbs = 1;
    for (const p of calPoints) {
        maxAbs = Math.max(maxAbs, Math.abs(p.x), Math.abs(p.y));
    }

    const pad = 18;
    const cx = w / 2;
    const cy = h / 2;
    const scale = (Math.min(w, h) / 2 - pad) / maxAbs;

    // Axes
    ctx.save();
    ctx.strokeStyle = "rgba(255,255,255,0.35)";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(cx, pad);
    ctx.lineTo(cx, h - pad);
    ctx.moveTo(pad, cy);
    ctx.lineTo(w - pad, cy);
    ctx.stroke();
    ctx.restore();

    // Points
    ctx.save();
    ctx.fillStyle = "rgba(0, 209, 255, 0.75)";
    for (const p of calPoints) {
        const px = cx + p.x * scale;
        const py = cy - p.y * scale;

        // skip if out of bounds
        if (px < 0 || px > w || py < 0 || py > h) continue;

        ctx.fillRect(px, py, 2, 2);
    }
    ctx.restore();
}

function updateCompensatorUi(xOff, yOff) {
    const xEl = qs("#compX");
    const yEl = qs("#compY");
    if (xEl) xEl.textContent = String(xOff);
    if (yEl) yEl.textContent = String(yOff);
}

function handleBleNotificationBytes(bytes) {
    if (!(bytes instanceof Uint8Array) || bytes.length < 1) return;

    const type = bytes[0];

    if (type === TYPE_TX_CAL_POINT) {
        if(!calibrating) return;
        if (bytes.length < 7) return;
        const dv = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
        const x = dv.getInt16(1, true);
        const y = dv.getInt16(3, true);
        const z = dv.getInt16(5, true);

        calPoints.push({ x, y, z, t: Date.now() });
        if (calPoints.length > calMaxPoints) {
            calPoints.splice(0, calPoints.length - calMaxPoints);
        }
        drawCalibrationScatter();
        return;
    }

    if (type === TYPE_TX_COMPENSATOR) {
        if (bytes.length < 5) return;
        const dv = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
        const xOff = dv.getInt16(1, true);
        const yOff = dv.getInt16(3, true);
        updateCompensatorUi(xOff, yOff);
        return;
    }
}

async function connectBLE() {
    try {
        await bleConnect();
        bleSubscribeNotifications(handleBleNotificationBytes);
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

    // little endian = true for ESP32
    view.setFloat64(1, lat, true);
    view.setFloat64(9, lon, true);

    const err = await bleSendFrame(buffer);
    if (err === 0) {
        showToast("Waypoint sent.", "ok");
    }
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

    const err = await bleSendFrame(buffer);
    if (err === 0) {
        showToast("Calibration started.", "ok");
    }
}

async function stopSaveCalibration() {
    const buffer = new ArrayBuffer(1);
    const view = new DataView(buffer);

    view.setUint8(0, TYPE_STOP_SAVE_CALIBRATION);

    const err = await bleSendFrame(buffer);
    if (err === 0) {
        showToast("Calibration done.", "ok");
    }
}

async function stopDiscardCalibration() {
    const buffer = new ArrayBuffer(1);
    const view = new DataView(buffer);
    
    view.setUint8(0, TYPE_STOP_DISCARD_CALIBRATION);
    
    const err = await bleSendFrame(buffer);
    if (err === 0) {
        showToast("Calibration discarded.", "ok");
    }
}

function updateCalibrationButtonUi() {
    const btn = qs("#btnToggleCalibrateLive");
    if (!btn) return;
    btn.setAttribute("aria-pressed", calibrating ? "true" : "false");
    btn.textContent = calibrating ? "Stop calibration" : "Start calibration";
}

function toggleCalibrationLive() {
    calibrating = !calibrating;

    if (calibrating) {
        startCalibration();
    } else {
        clearCalibrationPlot();
        stopSaveCalibration();
    }

    updateCalibrationButtonUi();
    setStatusLine();
}


function cancelCalibrationLive() {
    if (!calibrating) return;
    calibrating = false;
    stopDiscardCalibration();
    clearCalibrationPlot();
    updateCalibrationButtonUi();
    setStatusLine();
}

function clearCalibrationPlot() {
    calPoints = [];
    drawCalibrationScatter();
}

function initUi() {
    const btnGoMenu = qs("#btnGoMenu");
    const btnBackToMenu = qs("#btnBackToMenu");
    const btnOpenMap = qs("#btnOpenMap");
    const btnCenterMarker = qs("#btnCenterMarker");
    const btnOpenCalibration = qs("#btnOpenCalibration");

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

    btnOpenCalibration.addEventListener("click", () => {
        showView("#calibrationView");
        // draw once
        drawCalibrationScatter();
        setStatusLine();
    });

    qs("#btnConnectBle").addEventListener("click", connectBLE);

    // Map actions
    qs("#btnSendWaypoint").addEventListener("click", sendWaypoint);
    qs("#btnSaveWaypoint").addEventListener("click", saveWaypoint);
    qs("#btnLoadWaypoint").addEventListener("click", loadWaypoint);
    qs("#btnRenameWaypoint").addEventListener("click", renameWaypoint);
    qs("#btnDeleteWaypoint").addEventListener("click", deleteWaypoint);
    btnCenterMarker.addEventListener("click", centerOnMarker);

    // Calibration actions
    qs("#btnBackToMenuCal").addEventListener("click", () => {
        cancelCalibrationLive();
        showView("#menuView");
    });
    qs("#btnToggleCalibrateLive").addEventListener("click", toggleCalibrationLive);
    qs("#btnClearCalPlot").addEventListener("click", clearCalibrationPlot);

    // Start on menu
    updateCalibrationButtonUi();
    showView("#menuView");
    setStatusLine();

    window.addEventListener("resize", () => {
        if (qs("#calibrationView").classList.contains("viewActive")) {
            drawCalibrationScatter();
        }
    });
}

document.addEventListener("DOMContentLoaded", initUi);
