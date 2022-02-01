import random
import time
import struct

from paho.mqtt import client as mqtt_client


#broker = 'localhost'
broker = 'test.mosquitto.org'
port = 1883
topic = "865235030717330"
# generate client ID with pub prefix randomly
client_id = f'python-mqtt-{random.randint(0, 1000)}'

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port, keepalive = 3600)
    return client


def publish(client):
    msg_count = 0
    while True:
        #msg = struct.pack("BBBBHHfff", msg_count, 1, 0, 4, 4123, 300, 21.38810, -77.91893, 8.33)
        msg = "0,1,0,25.72939,-93.64918,3,300,6,4151"
        result = client.publish(topic, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Send `{msg_count}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")
        msg_count += 1
        time.sleep(5)


def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)


if __name__ == '__main__':
    run()
