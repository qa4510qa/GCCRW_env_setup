#!/usr/bin/env python3
import datetime
import paho.mqtt.client as mqtt

now = datetime.datetime.now()
# This is the Subscriber
time = datetime.datetime.now().strftime("%H:%M")
date = datetime.datetime.now().strftime("%Y_%m_%d")


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    #client.subscribe("MCU_00/#")
    client.subscribe("MCU_01/#")
    client.subscribe("MCU_02/#")
    client.subscribe("MCU_03/#")
    client.subscribe("MCU_04/#")
    client.subscribe("MCU_05/#")
    client.subscribe("MCU_06/#")
    client.subscribe("MCU_07/#")
    client.subscribe("MCU_08/#")
    client.subscribe("MCU_09/#")
    client.subscribe("MCU_10/#")
    client.subscribe("MCU_11/#")
    client.subscribe("MCU_12/#")
    client.subscribe("MCU_13/#")
    client.subscribe("MCU_14/#")

def on_message(client, userdata, msg):
    time = datetime.datetime.now().strftime("%H:%M")
    date = datetime.datetime.now().strftime("%Y_%m_%d")
    print(time[4:5] == "3")
    print(datetime.datetime.now())
    print(msg.topic+":"+msg.payload.decode())
    #print(type(msg.topic[0:6]))
    with open("{}_{}.txt".format(msg.topic[0:6],date),"a") as f:
        f.write("{}, {}: {}\n".format(time, msg.topic, msg.payload.decode()))
    #fp = open("MCU_01_{}.txt".format(date),"a")
    #fp.write("{}, {}: {}".format(time, msg.topic, msg.payload.decode()))
    #fp.close()

client = mqtt.Client()
client.connect("192.168.1.100",1883,60)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()