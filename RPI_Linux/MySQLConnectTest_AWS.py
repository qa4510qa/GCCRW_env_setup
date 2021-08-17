import datetime
import mysql.connector
from mysql.connector import Error

try:
    connection = mysql.connector.connect(host='',
                                         port='3306',
                                         user='',
                                         database='',
                                         password='')
    if connection.is_connected():
        db_Info = connection.get_server_info()
        print("Connected to MySQL Server version ", db_Info)
        cursor = connection.cursor()
        cursor.execute("select database();")
        record = cursor.fetchone()
        print("You're connected to database: ", record)
        
    sql = "INSERT INTO env_data (temp, humid, s_humid, time, descr) VALUES (%s, %s, %s, %s, %s);"
    new_data = (25, 75, 50, datetime.datetime.now(), "test data")
    cursor = connection.cursor()
    cursor.execute(sql, new_data)
    
    connection.commit()



except Error as e:
    print("Error while connecting to MySQL", e)
    
finally:
    if (connection.is_connected()):
        cursor.close()
        connection.close()
        print("MySQL connection is closed")
        
#CREATE TABLE env_data(id INT NOT NULL AUTO_INCREMENT, temp INT NOT NULL, humid INT NOT NULL, s_humid INT NOT NULL, time varchar(30) NOT NULL, descr varchar(50), PRIMARY KEY(id));