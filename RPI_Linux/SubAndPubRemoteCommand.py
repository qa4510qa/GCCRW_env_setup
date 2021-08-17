#!/usr/bin/env python3
import datetime
import paho.mqtt.client as mqtt

now = datetime.datetime.now()
time = datetime.datetime.now().strftime("%H:%M")
date = datetime.datetime.now().strftime("%Y_%m_%d")


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    #client.subscribe("MCU_00/#")
    client.subscribe("test")



def on_message(client, userdata, msg):
    client_pub = mqtt.Client()
    client_pub.username_pw_set("","")  # User, Password
    client_pub.connect("192.168.1.100", 1883, 60)
    print(msg.topic+":"+msg.payload.decode())
    t = datetime.datetime.now().strftime("%H:%M:%S")
    #payload = {'Command' : msg.payload.decode() , 'Time' : t}
    #print (json.dumps(payload))
    client_pub.publish(msg.topic, msg.payload.decode())
    time.sleep(10)
    


client = mqtt.Client()
client.connect("140.112.63.231", 1883, 60)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()

