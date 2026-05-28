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

function bleNormalizeToArrayBuffer(frame) {
    if (frame instanceof ArrayBuffer) {
        return frame;
    }

    // DataView
    if (frame && frame.buffer instanceof ArrayBuffer) {
        // TypedArray or DataView
        if (typeof frame.byteOffset === "number" && typeof frame.byteLength === "number") {
            return frame.buffer.slice(frame.byteOffset, frame.byteOffset + frame.byteLength);
        }
        return frame.buffer;
    }

    throw new Error("Unsupported frame type. Use ArrayBuffer, TypedArray, or DataView.");
}

async function bleSendFrame(frame) {
    if (!bleCharacteristic) {
        throw new Error("BLE not connected.");
    }

    const buffer = bleNormalizeToArrayBuffer(frame);

    // Web Bluetooth accepts BufferSource; ArrayBuffer is ok
    await bleCharacteristic.writeValue(buffer);
}
