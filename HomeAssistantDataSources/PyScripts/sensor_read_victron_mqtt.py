import asyncio
import json

import paho.mqtt.client as mqtt
from victron_ble.scanner import Scanner


# =========================
# KONFIGURACJA
# =========================

VICTRON_ADDRESS = ""

# Wklej tutaj swój prawdziwy Advertisement Key
VICTRON_KEY = ""

MQTT_HOST = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC = "victron/mppt15035/state"


# =========================
# MQTT
# =========================

mqtt_client = mqtt.Client(
    callback_api_version=mqtt.CallbackAPIVersion.VERSION2
)

mqtt_client.connect(MQTT_HOST, MQTT_PORT, 60)
mqtt_client.loop_start()


# =========================
# VICtron BLE
# =========================

class MyScanner(Scanner):

    def __init__(self):
        super().__init__(
            {
                VICTRON_ADDRESS.lower(): VICTRON_KEY
            },
            indent=None,
        )
    def publish_discovery(self):
        sensors = {
            "battery_voltage": {
                "name": "Napięcie akumulatora",
                "unit": "V",
                "device_class": "voltage",
                "state_class": "measurement",
            },
            "battery_charging_current": {
                "name": "Prąd ładowania",
                "unit": "A",
                "device_class": "current",
                "state_class": "measurement",
            },
            "solar_power": {
                "name": "Moc PV",
                "unit": "W",
                "device_class": "power",
                "state_class": "measurement",
            },
            "yield_today": {
                "name": "Produkcja dzisiaj",
                "unit": "Wh",
                "device_class": "energy",
                "state_class": "total_increasing",
            },
            "external_device_load": {
                "name": "Obciążenie",
                "unit": "A",
                "device_class": "current",
                "state_class": "measurement",
            },
            "charge_state": {
                "name": "Stan ładowania",
            },
            "charger_error": {
                "name": "Błąd ładowarki",
            },
        }

        for key, config in sensors.items():

            payload = {
                "name": config["name"],
                "unique_id": f"victron_mppt15035_{key}",
                "state_topic": MQTT_TOPIC,
                "value_template": f"{{{{ value_json.{key} }}}}",
                "device": {
                    "identifiers": ["victron_mppt15035"],
                    "name": "Victron SmartSolar MPPT 150/35",
                    "manufacturer": "Victron Energy",
                    "model": "SmartSolar MPPT 150/35",
                },
            }

            if "unit" in config:
                payload["unit_of_measurement"] = config["unit"]

            if "device_class" in config:
                payload["device_class"] = config["device_class"]

            if "state_class" in config:
                payload["state_class"] = config["state_class"]

            topic = (
                f"homeassistant/sensor/"
                f"victron_mppt15035/{key}/config"
            )

            mqtt_client.publish(
                topic,
                json.dumps(payload),
                qos=0,
                retain=True,
            )

    def callback(self, ble_device, raw_data, advertisement):

        if ble_device.address.lower() != VICTRON_ADDRESS.lower():
            return

        try:
            device = self.get_device(
                ble_device,
                raw_data
            )

            data = device.parse(raw_data)

            payload = {
                "model_name": data.get_model_name(),
                "battery_voltage": data.get_battery_voltage(),
                "battery_charging_current": data.get_battery_charging_current(),
                "solar_power": data.get_solar_power(),
                "yield_today": data.get_yield_today(),
                "external_device_load": data.get_external_device_load(),
                "charge_state": data.get_charge_state(),
                "charger_error": data.get_charger_error(),
            }

            # Enum → tekst
            if payload["charge_state"] is not None:
                payload["charge_state"] = (
                    payload["charge_state"].name.lower()
                )

            if payload["charger_error"] is not None:
                payload["charger_error"] = (
                    payload["charger_error"].name.lower()
                )

            message = json.dumps(payload)

            print(message)

            result = mqtt_client.publish(
                MQTT_TOPIC,
                message,
                qos=0,
                retain=True,
            )

            if result.rc == mqtt.MQTT_ERR_SUCCESS:
                print("→ MQTT OK")
            else:
                print(f"→ MQTT ERROR: {result.rc}")

        except Exception as e:
            print(f"Błąd odczytu Victron: {e}")


# =========================
# MAIN
# =========================

async def main():

    print("Uruchamiam Victron → MQTT...")
    print(f"Adres: {VICTRON_ADDRESS}")
    print(f"MQTT: {MQTT_HOST}:{MQTT_PORT}")
    print()

    scanner = MyScanner()
    scanner.publish_discovery()

    await scanner.start()

    try:
        while True:
            await asyncio.sleep(1)

    except KeyboardInterrupt:
        print("\nZatrzymuję...")

    finally:
        await scanner.stop()
        mqtt_client.loop_stop()
        mqtt_client.disconnect()


asyncio.run(main())