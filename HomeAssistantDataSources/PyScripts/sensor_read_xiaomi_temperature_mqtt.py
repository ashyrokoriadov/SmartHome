import asyncio
import json
from datetime import datetime

from bleak import BleakScanner
from habluetooth import BluetoothServiceInfoBleak
from xiaomi_ble import XiaomiBluetoothDeviceData
import paho.mqtt.client as mqtt


MQTT_HOST = "localhost"
MQTT_PORT = 1883

DISCOVERY_PREFIX = "homeassistant"


SENSORS = [
    {
        "mac": "",
        "bindkey": "",
        "name": "Xiaomi LYWSD03MMC — Bathroom",
        "topic": "xiaomi/bathroom/lywsd03mmc/state",
        "unique_id_prefix": "xiaomi_lywsd03mmc_bathroom",
    },
    {
        "mac": "",
        "bindkey": "",
        "name": "Xiaomi LYWSD03MMC - Leaving room",
        "topic": "xiaomi/leaving_room/lywsd03mmc/state",
        "unique_id_prefix": "xiaomi_lywsd03mmc_leaving_room",
    },
    {
        "mac": "",
        "bindkey": "",
        "name": "Xiaomi LYWSD03MMC - Home office",
        "topic": "xiaomi/home_office/lywsd03mmc/state",
        "unique_id_prefix": "xiaomi_lywsd03mmc_home_office",
    },
]


def publish_discovery(client, sensor):
    """Create Home Assistant MQTT Discovery entities."""

    sensors = [
        {
            "key": "temperature",
            "name": "Temperature",
            "device_class": "temperature",
            "unit": "°C",
            "value_template": "{{ value_json.temperature }}",
        },
        {
            "key": "humidity",
            "name": "Humidity",
            "device_class": "humidity",
            "unit": "%",
            "value_template": "{{ value_json.humidity }}",
        },
        {
            "key": "signal_strength",
            "name": "Signal Strength",
            "device_class": "signal_strength",
            "unit": "dBm",
            "value_template": "{{ value_json.signal_strength }}",
        },
    ]

    for entity in sensors:

        topic = (
            f"{DISCOVERY_PREFIX}/sensor/"
            f"{sensor['unique_id_prefix']}_{entity['key']}/config"
        )

        payload = {
            "name": entity["name"],
            "unique_id": (
                f"{sensor['unique_id_prefix']}_{entity['key']}"
            ),
            "state_topic": sensor["topic"],
            "value_template": entity["value_template"],
            "device_class": entity["device_class"],
            "unit_of_measurement": entity["unit"],
            "state_class": "measurement",
            "device": {
                "identifiers": [
                    sensor["unique_id_prefix"]
                ],
                "name": sensor["name"],
                "manufacturer": "Xiaomi",
                "model": "LYWSD03MMC",
            },
        }

        client.publish(
            topic,
            json.dumps(payload),
            retain=True,
        )

        print("Discovery:", topic)


def main():

    print("Connecting to MQTT...")

    mqtt_client = mqtt.Client(
        mqtt.CallbackAPIVersion.VERSION2,
        client_id="xiaomi-lywsd03mmc-multi",
    )

    mqtt_client.connect(
        MQTT_HOST,
        MQTT_PORT,
        60,
    )

    mqtt_client.loop_start()

    print(
        f"Connected to MQTT broker "
        f"{MQTT_HOST}:{MQTT_PORT}"
    )

    print()
    print("Publishing Home Assistant Discovery...")

    for sensor in SENSORS:
        publish_discovery(
            mqtt_client,
            sensor,
        )

    # Create one Xiaomi parser for each sensor
    parsers = {}

    # Store latest values independently for every sensor
    latest_values = {}

    for sensor in SENSORS:

        mac = sensor["mac"].upper()

        parsers[mac] = XiaomiBluetoothDeviceData(
            bytes.fromhex(sensor["bindkey"])
        )

        latest_values[mac] = {}

    print()
    print("Configured Xiaomi sensors:")

    for sensor in SENSORS:

        print(
            f"  {sensor['name']}"
        )

        print(
            f"    MAC:   {sensor['mac']}"
        )

        print(
            f"    MQTT:  {sensor['topic']}"
        )

    print()
    print("Scanning...")
    print("Press Ctrl+C to stop.")
    print()

    def detection_callback(device, advertisement_data):

        mac = device.address.upper()

        # Ignore all BLE devices that are not our Xiaomi sensors
        if mac not in parsers:
            return

        sensor = next(
            s for s in SENSORS
            if s["mac"].upper() == mac
        )

        parser = parsers[mac]

        try:

            service_info = BluetoothServiceInfoBleak(
                name=device.name or "",
                address=device.address,
                rssi=advertisement_data.rssi,
                manufacturer_data=advertisement_data.manufacturer_data,
                service_data=advertisement_data.service_data,
                service_uuids=advertisement_data.service_uuids,
                source="local",
                device=device,
                advertisement=advertisement_data,
                connectable=True,
                time=0,
                tx_power=advertisement_data.tx_power or 0,
                raw=None,
            )

            update = parser.update(service_info)

            changed = False

            for key, value in update.entity_values.items():

                if key.key == "temperature":

                    latest_values[mac]["temperature"] = (
                        value.native_value
                    )

                    changed = True

                elif key.key == "humidity":

                    latest_values[mac]["humidity"] = (
                        value.native_value
                    )

                    changed = True

                elif key.key == "signal_strength":

                    latest_values[mac]["signal_strength"] = (
                        value.native_value
                    )

                    changed = True

            if not changed:
                return

            now = datetime.now().strftime(
                "%Y-%m-%d %H:%M:%S"
            )

            values = latest_values[mac]

            print(
                f"[{now}] "
                f"{sensor['name']} | "
                f"Temperature: "
                f"{values.get('temperature', '-')}, "
                f"Humidity: "
                f"{values.get('humidity', '-')}, "
                f"RSSI: "
                f"{values.get('signal_strength', '-')}"
            )

            payload = json.dumps(
                values,
                separators=(",", ":"),
            )

            mqtt_client.publish(
                sensor["topic"],
                payload,
                retain=True,
            )

            print(
                "MQTT:",
                sensor["topic"],
                payload,
            )

        except Exception as e:

            print(
                f"Parser error for "
                f"{sensor['name']}:",
                repr(e),
            )

    async def run_scanner():

        scanner = BleakScanner(
            detection_callback=detection_callback
        )

        await scanner.start()

        try:

            while True:

                await asyncio.sleep(1)

        finally:

            await scanner.stop()

    try:

        asyncio.run(run_scanner())

    except KeyboardInterrupt:

        print()
        print("Stopping...")

    finally:

        mqtt_client.loop_stop()
        mqtt_client.disconnect()

        print("Stopped.")


if __name__ == "__main__":
    main()