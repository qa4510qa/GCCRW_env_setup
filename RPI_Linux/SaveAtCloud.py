#!/usr/bin/env python3
import time
import datetime
import mysql.connector
from mysql.connector import Error

def data_uploading(upload_data):
    print(upload_data)
    try:
        connection = mysql.connector.connect(host='',
                                            port='',
                                            database='',
                                            user='',
                                            password='')
        if connection.is_connected():
            recording("MySQL connection connected", datetime.datetime.now())
            db_Info = connection.get_server_info()
            print("Connected to MySQL Server version ", db_Info)
            cursor = connection.cursor()
            cursor.execute("select database();")
            record = cursor.fetchone()
            print("You're connected to database: ", record)
        if upload_data["temp_high"] != "":    
            sql = "INSERT INTO `greenhouse_env_data`(`position` ,`temp_high`, `temp_low`, `humid_high`, `humid_low`, `soil_humid`, `time`, `remarks_1`) VALUES (%s,%s,%s,%s,%s,%s,%s,%s)"
            new_data = (upload_data["position"], upload_data["temp_high"], upload_data["temp_low"], upload_data["humid_high"], upload_data["humid_low"], upload_data["soil_humid"], upload_data["time"], upload_data["remarks_1"])
        else:
            sql = "INSERT INTO `greenhouse_env_data`(`position` , `temp_low`, `humid_low`, `soil_humid`, `time`, `remarks_1`) VALUES (%s,%s,%s,%s,%s,%s)"
            new_data = (upload_data["position"], upload_data["temp_low"], upload_data["humid_low"], upload_data["soil_humid"], upload_data["time"], upload_data["remarks_1"])

        cursor = connection.cursor()
        cursor.execute(sql, new_data)
        
        connection.commit()

    except Error as e:
        error_handling("Error while connecting to MySQL: {}".format(e))
        print("Error while connecting to MySQL", e)
        
    finally:
        if (connection.is_connected()):
            cursor.close()
            connection.close()
            recording("MySQL connection closed", datetime.datetime.now())
            print("MySQL connection is closed")

        
def position_without_high_sensor():
    station = ["MCU_01","MCU_02","MCU_03","MCU_04","MCU_05","MCU_06","MCU_07","MCU_08"]
    #if now_time[4:5] == "1":  #connect to the cloud every minute
    for i in range(len(station)):
        upload_data = {"position":station[i], "temp_high":"" , "temp_low":0, "humid_high":"", "humid_low":0, "soil_humid":0, "time":now, "remarks_1":""}
        try:
            with open("{}_{}.txt".format(station[i], now_date), "rb") as f:
                a = f.readline()
                off = -120
                while True:
                    f.seek(off,2)
                    lines = f.readlines()
                    if len(lines)>=4 and lines[-1].decode()[-1]!="!" and lines[-2].decode()[-1]!="!" and lines[-3].decode()[-1]!="!":
                        first_line = lines[-3]
                        second_line = lines[-2]
                        third_line = lines[-1]
                        break
                    off*=2
        
            if first_line.decode()[14:15] == "t":
                print("test1")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = first_line.decode()[-6:-1]
                upload_data["humid_low"] = second_line.decode()[-6:-1]
                upload_data["soil_humid"] = third_line.decode()[-6:-1]
                data_uploading(upload_data)
            elif first_line.decode()[14:15] == "h":
                print("test2")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = third_line.decode()[-6:-1]
                upload_data["humid_low"] = first_line.decode()[-6:-1]
                upload_data["soil_humid"] = second_line.decode()[-6:-1]
                data_uploading(upload_data)
            elif first_line.decode()[14:15] == "s":
                print("test3")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = second_line.decode()[-6:-1]
                upload_data["humid_low"] = third_line.decode()[-6:-1]
                upload_data["soil_humid"] = first_line.decode()[-6:-1]
                data_uploading(upload_data)
            else:
                print("read line error!")
                error_handling("unexpected line: {}".format(first_line.decode()))
                print(first_line.decode()[14:15]) 
        except IOError as e:
            error_handling("Error while opening file: {}".format(e))
            print("Error while opening file", e)


def position_with_high_sensor():
    station = ["MCU_09","MCU_10","MCU_11","MCU_12","MCU_13","MCU_14"]
    #if now_time[4:5] == "1":  #connect to the cloud every minute
    for i in range(len(station)):
        upload_data = {"position":station[i], "temp_high":0, "temp_low":0, "humid_high":0, "humid_low":0, "soil_humid":0, "time":now, "remarks_1":""}
        try:
            with open("{}_{}.txt".format(station[i], now_date), "rb") as f:
                a = f.readline()
                off = -240
                while True:
                    f.seek(off,2)
                    lines = f.readlines()
                    if len(lines)>=6 and lines[-1].decode()[-1]!="!" and lines[-2].decode()[-1]!="!" and lines[-3].decode()[-1]!="!" and lines[-4].decode()[-1]!="!" and lines[-5].decode()[-1]!="!":
                        first_line = lines[-5]
                        second_line = lines[-4]
                        third_line = lines[-3]
                        fourth_line = lines[-2]
                        fifth_line = lines[-1]
                        break
                    off*=2

            if first_line.decode()[14:15] == "t" and third_line.decode()[14:15] == "s":
                print("test1")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = first_line.decode()[-6:-1]
                upload_data["humid_low"] = second_line.decode()[-6:-1]
                upload_data["soil_humid"] = third_line.decode()[-6:-1]
                upload_data["temp_high"] = fourth_line.decode()[-6:-1]
                upload_data["humid_high"] = fifth_line.decode()[-6:-1]
                data_uploading(upload_data)
            elif first_line.decode()[14:15] == "h" and second_line.decode()[14:15] == "s":
                print("test2")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = fifth_line.decode()[-6:-1]
                upload_data["humid_low"] = first_line.decode()[-6:-1]
                upload_data["soil_humid"] = second_line.decode()[-6:-1]
                upload_data["temp_high"] = third_line.decode()[-6:-1]
                upload_data["humid_high"] = fourth_line.decode()[-6:-1]
                data_uploading(upload_data)
            elif first_line.decode()[14:15] == "s":
                print("test3")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = fourth_line.decode()[-6:-1]
                upload_data["humid_low"] = fifth_line.decode()[-6:-1]
                upload_data["soil_humid"] = first_line.decode()[-6:-1]
                upload_data["temp_high"] = second_line.decode()[-6:-1]
                upload_data["humid_high"] = third_line.decode()[-6:-1]
                data_uploading(upload_data)
            elif first_line.decode()[14:15] == "t" and third_line.decode()[14:15] == "t":
                print("test4")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = third_line.decode()[-6:-1]
                upload_data["humid_low"] = fourth_line.decode()[-6:-1]
                upload_data["soil_humid"] = fifth_line.decode()[-6:-1]
                upload_data["temp_high"] = first_line.decode()[-6:-1]
                upload_data["humid_high"] = second_line.decode()[-6:-1]
                data_uploading(upload_data)
            elif first_line.decode()[14:15] == "h" and second_line.decode()[14:15] == "t":
                print("test5")
                upload_data["time"] = first_line.decode()[0:5]
                upload_data["temp_low"] = fifth_line.decode()[-6:-1]
                upload_data["humid_low"] = first_line.decode()[-6:-1]
                upload_data["soil_humid"] = second_line.decode()[-6:-1]
                upload_data["temp_high"] = third_line.decode()[-6:-1]
                upload_data["humid_high"] = fourth_line.decode()[-6:-1]
                data_uploading(upload_data)
            else:
                print("read line error!")
                error_handling("unexpected line: {}".format(first_line.decode()))
                print(first_line.decode())
        except IOError as e:
            error_handling("Error while opening file: {}".format(e))
            print("Error while opening file", e)

def recording(msg, time):
    with open("record_{}.txt".format(now_date[0:7]),"a") as f:
        f.write("{} at: {}\n".format(msg, time))

def error_handling(e):
    recording("{}".format(e), datetime.datetime.now())

while True:
    now = datetime.datetime.now().strftime("%Y_%m_%d %H:%M")
    now_time = datetime.datetime.now().strftime("%H:%M")
    now_date = datetime.datetime.now().strftime("%Y_%m_%d")
    print(datetime.datetime.now())
    if now_time[4:5] == "1":  #connect to the cloud every 10 minutes
        position_without_high_sensor()
        position_with_high_sensor()


    time.sleep(60)