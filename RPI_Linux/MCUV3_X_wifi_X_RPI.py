import time
import paho.mqtt.client as mqtt

# Don't forget to change the variables for the MQTT broker!
mqtt_broker_ip = ""
client=mqtt.Client()

def on_connect(client, userdata, flags, rc):
    print("Connected with result code" + str(rc))
    client.subscribe("MCU_01/#")
    
def on_message(client, userdata, message):
    # Show status of sensors  
    #room1
    print(msg.topic.decode())
    print(msg.payload.decode())

    if msg.topic == "MCU_01/status":
        if msg.payload == "1":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_01/status":
        if msg.payload == "0":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    #room2
    if msg.topic == "MCU_02/status":
        if msg.payload == "1":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_02/status":
        if msg.payload == "0":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    #room3
    if msg.topic == "MCU_03/status":
        if msg.payload == "1":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_03/status":
        if msg.payload == "0":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    #room4
    if msg.topic == "MCU_04/status":
        if msg.payload == "1":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_04/status":
        if msg.payload == "0":
            print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    
    #Print humidity and temperature values
    if msg.topic == "MCU_01/temperature":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_01/humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_01/soil_humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_02/temperature":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_02/humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_02/soil_humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_03/temperature":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_03/humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_03/soil_humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_04/temperature":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_04/humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))
    if msg.topic == "MCU_04/soil_humidity":
        print ("Topic: ", msg.topic + "\nMessage: " + str(msg.payload))            
            
client.on_connect = on_connect
client.on_message = on_message
client.connect(mqtt_broker_ip, 1883)  
client.loop_forever()
time.sleep(30)


            