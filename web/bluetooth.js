/*
 * Bluetooth module (loaded before app.js)
 * Exposes:
 *   - bleConnect(): connect and find our RX/TX characteristics by UUID
 *   - bleIsConnected(): boolean
 *   - bleSendFrame(buffer): async send ArrayBuffer/TypedArray/DataView to RX characteristic
 *   - bleSubscribeNotifications(onBytes): registers callback for TX notifications
 *   - bleGetRxCharacteristic(): returns RX characteristic (or null)
 *   - bleGetTxCharacteristic(): returns TX characteristic (or null)
 */

let bleDevice = null;
let bleServer = null;
let bleRxCharacteristic = null;
let bleTxCharacteristic = null;

let bleNotifyHandler = null;

function bleGetRxCharacteristic() {
    return bleRxCharacteristic;
}

function bleGetTxCharacteristic() {
    return bleTxCharacteristic;
}

function bleIsConnected() {
    return !!bleRxCharacteristic && !!bleTxCharacteristic;
}

async function bleConnect() {
    bleDevice = null;
    bleServer = null;
    bleRxCharacteristic = null;
    bleTxCharacteristic = null;

    bleDevice = await navigator.bluetooth.requestDevice({
        filters: [{ services: [BLE_SERVICE_UUID] }]
    });

    bleServer = await bleDevice.gatt.connect();

    const service = await bleServer.getPrimaryService(BLE_SERVICE_UUID);

    bleRxCharacteristic = await service.getCharacteristic(BLE_RX_CHAR_UUID);
    bleTxCharacteristic = await service.getCharacteristic(BLE_TX_CHAR_UUID);

    // Setup notifications
    await bleTxCharacteristic.startNotifications();
    bleTxCharacteristic.addEventListener("characteristicvaluechanged", (event) => {
        const value = event.target.value; // DataView
        const bytes = new Uint8Array(value.buffer, value.byteOffset, value.byteLength);
        if (typeof bleNotifyHandler === "function") {
            // Copy to detach from underlying buffer reuse
            bleNotifyHandler(new Uint8Array(bytes));
        }
    });

    return { rx: bleRxCharacteristic, tx: bleTxCharacteristic };
}

function bleSubscribeNotifications(onBytes) {
    bleNotifyHandler = onBytes;
}

async function bleSendFrame(buffer) {
    if (!bleIsConnected()) {
        showToast("Not connected to BLE.", "error");
        return 1;
    }
    try {
        // Web Bluetooth accepts BufferSource; DataView is also ok.
        await bleRxCharacteristic.writeValue(buffer);
        return 0;
    } catch (e) {
        showToast("Send failed: " + e, "error", 3500);
        return 1;
    }
}
