/*
 * Bluetooth module (loaded before app.js)
 * Exposes:
 *   - bleConnect(): connect and find first writable characteristic
 *   - bleIsConnected(): boolean
 *   - bleSendFrame(buffer): async send ArrayBuffer/TypedArray/DataView
 *   - bleGetCharacteristic(): returns the current characteristic (or null)
 */

let bleDevice = null;
let bleCharacteristic = null;

function bleGetCharacteristic() {
    return bleCharacteristic;
}

function bleIsConnected() {
    return !!bleCharacteristic;
}

async function bleConnect() {
    bleDevice = null;
    bleCharacteristic = null;

    bleDevice = await navigator.bluetooth.requestDevice({
        acceptAllDevices: true
    });

    const server = await bleDevice.gatt.connect();

    const services = await server.getPrimaryServices();
    for (const service of services) {
        try {
            const characteristics = await service.getCharacteristics();
            for (const char of characteristics) {
                if (char.properties.write || char.properties.writeWithoutResponse) {
                    bleCharacteristic = char;
                    return bleCharacteristic;
                }
            }
        } catch (e) {
            // continue scanning other services
        }
    }

    throw new Error("No writable BLE characteristic found.");
}


async function bleSendFrame(buffer) {
    if (!bleIsConnected()) {
        showToast("Not connected to BLE.", "error");
        return;
    }
    try {
        await bleCharacteristic.writeValue(buffer);
    } catch (e) {
        showToast("Send failed: " + e, "error", 3500);
    }
}


