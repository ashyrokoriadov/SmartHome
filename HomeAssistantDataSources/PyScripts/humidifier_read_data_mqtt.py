import json
import time

import paho.mqtt.client as mqtt

from miio import AirHumidifierMiot
from miio.integrations.humidifier.zhimi.airhumidifier_miot import OperationMode


# ============================================================
# Smartmi Evaporative Humidifier 2
# ============================================================

DEVICE_IP = ""
DEVICE_TOKEN = ""

DEVICE_NAME = "Smartmi Evaporative Humidifier 2"
DEVICE_ID = "smartmi_humidifier_ca4"


# ============================================================
# MQTT
# ============================================================

MQTT_HOST = "localhost"
MQTT_PORT = 1883

STATE_TOPIC = "home/smartmi/humidifier"

COMMAND_POWER_TOPIC = "home/smartmi/humidifier/set/power"
COMMAND_TARGET_TOPIC = "home/smartmi/humidifier/set/target_humidity"
COMMAND_MODE_TOPIC = "home/smartmi/humidifier/set/mode"

DISCOVERY_PREFIX = "homeassistant"

UPDATE_INTERVAL = 30


# ============================================================
# Global objects
# ============================================================

device = None
client = None


# ============================================================
# Device connection
# ============================================================

def get_device():

    global device

    if device is None:

        device = AirHumidifierMiot(
            DEVICE_IP,
            DEVICE_TOKEN
        )

    return device


# ============================================================
# Read humidifier status
# ============================================================

def get_humidifier_data():

    status = get_device().status()

    # Convert python-miio enum to nice Home Assistant names
    mode_map = {

        "OperationMode.Auto":
            "Auto",

        "OperationMode.Low":
            "Low",

        "OperationMode.Mid":
            "Medium",

        "OperationMode.High":
            "High",
    }

    mode = mode_map.get(
        str(status.mode),
        "Unknown"
    )

    return {

        "temperature":
            status.temperature,

        "humidity":
            status.humidity,

        "target_humidity":
            status.target_humidity,

        "water_level":
            status.water_level,

        "power":
            str(status.power)
                .replace("Power.", "")
                .lower(),

        "mode":
            mode,

        "motor_speed":
            status.motor_speed,

        "actual_speed":
            status.actual_speed,

        "water_tank_detached":
            status.water_tank_detached,

        "child_lock":
            status.child_lock,

        "dry":
            status.dry,

        "error":
            status.error,
    }


# ============================================================
# Publish current state
# ============================================================

def publish_state():

    try:

        data = get_humidifier_data()

        payload = json.dumps(data)

        client.publish(

            STATE_TOPIC,

            payload,

            retain=True
        )

        print(payload)

    except Exception as e:

        print(
            f"STATUS ERROR: {e}"
        )


# ============================================================
# MQTT command handler
# ============================================================

def on_message(client, userdata, message):

    command = (
        message.payload
        .decode()
        .strip()
    )

    print(
        f"MQTT command: "
        f"{message.topic} -> {command}"
    )

    try:

        device = get_device()


        # ====================================================
        # Power
        # ====================================================

        if message.topic == COMMAND_POWER_TOPIC:

            if command.upper() == "ON":

                device.on()

                print(
                    "Humidifier ON"
                )

            elif command.upper() == "OFF":

                device.off()

                print(
                    "Humidifier OFF"
                )

            else:

                print(
                    f"Unknown power command: "
                    f"{command}"
                )


        # ====================================================
        # Target humidity
        # ====================================================

        elif message.topic == COMMAND_TARGET_TOPIC:

            humidity = int(command)

            if 30 <= humidity <= 80:

                device.set_target_humidity(
                    humidity
                )

                print(
                    f"Target humidity set to "
                    f"{humidity}%"
                )

            else:

                print(
                    "Target humidity must be "
                    "between 30 and 80"
                )


        # ====================================================
        # Operating mode
        # ====================================================

        elif message.topic == COMMAND_MODE_TOPIC:

            mode_map = {

                "auto":
                    OperationMode.Auto,

                "low":
                    OperationMode.Low,

                "medium":
                    OperationMode.Mid,

                "high":
                    OperationMode.High,
            }

            mode = command.lower()

            if mode in mode_map:

                device.set_mode(
                    mode_map[mode]
                )

                print(
                    f"Mode set to {command}"
                )

            else:

                print(
                    f"Unknown mode: "
                    f"{command}"
                )


        # ====================================================
        # Refresh state after command
        # ====================================================

        time.sleep(1)

        publish_state()


    except Exception as e:

        print(
            f"COMMAND ERROR: {e}"
        )


# ============================================================
# MQTT Discovery
# ============================================================

def publish_discovery():

    device_config = {

        "identifiers":
            [DEVICE_ID],

        "name":
            DEVICE_NAME,

        "manufacturer":
            "Smartmi",

        "model":
            "Evaporative Humidifier 2",
    }


    # ========================================================
    # Sensors
    # ========================================================

    sensors = {

        "temperature": {

            "name":
                "Temperature",

            "device_class":
                "temperature",

            "unit_of_measurement":
                "°C",

            "value_template":
                "{{ value_json.temperature }}"
        },


        "humidity": {

            "name":
                "Humidity",

            "device_class":
                "humidity",

            "unit_of_measurement":
                "%",

            "value_template":
                "{{ value_json.humidity }}"
        },


        "target_humidity": {

            "name":
                "Target Humidity",

            "unit_of_measurement":
                "%",

            "value_template":
                "{{ value_json.target_humidity }}"
        },


        "water_level": {

            "name":
                "Water Level",

            "unit_of_measurement":
                "%",

            "value_template":
                "{{ value_json.water_level }}"
        },


        "motor_speed": {

            "name":
                "Motor Speed",

            "unit_of_measurement":
                "RPM",

            "value_template":
                "{{ value_json.motor_speed }}"
        },


        "actual_speed": {

            "name":
                "Actual Speed",

            "unit_of_measurement":
                "RPM",

            "value_template":
                "{{ value_json.actual_speed }}"
        },


        "power": {

            "name":
                "Power",

            "value_template":
                "{{ value_json.power }}"
        },


        "mode": {

            "name":
                "Mode",

            "value_template":
                "{{ value_json.mode }}"
        },


        "water_tank_detached": {

            "name":
                "Water Tank Detached",

            "value_template":
                "{{ value_json.water_tank_detached }}"
        },


        "child_lock": {

            "name":
                "Child Lock",

            "value_template":
                "{{ value_json.child_lock }}"
        },


        "dry": {

            "name":
                "Dry Mode",

            "value_template":
                "{{ value_json.dry }}"
        },


        "error": {

            "name":
                "Error",

            "value_template":
                "{{ value_json.error }}"
        },
    }


    for key, config in sensors.items():

        payload = {

            "name":
                config["name"],

            "unique_id":
                f"{DEVICE_ID}_{key}",

            "state_topic":
                STATE_TOPIC,

            "value_template":
                config["value_template"],

            "device":
                device_config,
        }


        if "device_class" in config:

            payload["device_class"] = (
                config["device_class"]
            )


        if "unit_of_measurement" in config:

            payload["unit_of_measurement"] = (
                config["unit_of_measurement"]
            )


        topic = (

            f"{DISCOVERY_PREFIX}/sensor/"
            f"{DEVICE_ID}/{key}/config"

        )


        client.publish(

            topic,

            json.dumps(payload),

            retain=True
        )


        print(
            f"Discovery published: {key}"
        )


    # ========================================================
    # Power switch
    # ========================================================

    power_payload = {

        "name":
            "Power",

        "unique_id":
            f"{DEVICE_ID}_power_switch",

        "state_topic":
            STATE_TOPIC,

        "value_template":
            "{{ value_json.power }}",

        "command_topic":
            COMMAND_POWER_TOPIC,

        "payload_on":
            "ON",

        "payload_off":
            "OFF",

        "state_on":
            "on",

        "state_off":
            "off",

        "device":
            device_config,
    }


    client.publish(

        f"{DISCOVERY_PREFIX}/switch/"
        f"{DEVICE_ID}/power/config",

        json.dumps(power_payload),

        retain=True
    )


    # ========================================================
    # Target humidity control
    # ========================================================

    humidity_payload = {

        "name":
            "Target Humidity Control",

        "unique_id":
            f"{DEVICE_ID}_target_humidity_control",

        "state_topic":
            STATE_TOPIC,

        "value_template":
            "{{ value_json.target_humidity }}",

        "command_topic":
            COMMAND_TARGET_TOPIC,

        "min":
            30,

        "max":
            80,

        "step":
            1,

        "unit_of_measurement":
            "%",

        "device_class":
            "humidity",

        "device":
            device_config,
    }


    client.publish(

        f"{DISCOVERY_PREFIX}/number/"
        f"{DEVICE_ID}/target_humidity/config",

        json.dumps(humidity_payload),

        retain=True
    )


    # ========================================================
    # Mode selector
    # ========================================================

    mode_payload = {

        "name":
            "Mode",

        "unique_id":
            f"{DEVICE_ID}_mode_control",

        "state_topic":
            STATE_TOPIC,

        "value_template":
            "{{ value_json.mode }}",

        "command_topic":
            COMMAND_MODE_TOPIC,

        "options": [

            "Auto",
            "Low",
            "Medium",
            "High"
        ],

        "device":
            device_config,
    }


    client.publish(

        f"{DISCOVERY_PREFIX}/select/"
        f"{DEVICE_ID}/mode/config",

        json.dumps(mode_payload),

        retain=True
    )


# ============================================================
# Main
# ============================================================

def main():

    global client

    client = mqtt.Client(

        mqtt.CallbackAPIVersion.VERSION2,

        client_id=
            "smartmi_humidifier"
    )


    client.on_message = on_message


    client.connect(

        MQTT_HOST,

        MQTT_PORT,

        60
    )


    # Subscribe to commands

    client.subscribe(
        COMMAND_POWER_TOPIC
    )

    client.subscribe(
        COMMAND_TARGET_TOPIC
    )

    client.subscribe(
        COMMAND_MODE_TOPIC
    )


    client.loop_start()


    print(
        "Smartmi MQTT publisher started"
    )


    # Register entities in Home Assistant

    publish_discovery()


    # ========================================================
    # Main loop
    # ========================================================

    while True:

        publish_state()

        time.sleep(
            UPDATE_INTERVAL
        )


# ============================================================
# Start
# ============================================================

if __name__ == "__main__":

    main()