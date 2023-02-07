import socket
import mysql.connector
from datetime import datetime

udp_socket = 5669 #UDP listening port
sql_username = 'sql_database_username'
sql_password = 'sql_database_password'
sql_database = 'sql_database_name'
sql_table_name = 'sql_table_name'


def create_udp_connection(udp_socket):
    '''

    :param udp_socket: integer - defined at top of script
    :return: client connection
    '''
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    client.bind(("",udp_socket))
    
    return client

    
if __name__ == '__main__':
    client = create_udp_connection(udp_socket)

    weather_db = mysql.connector.connect(
    host = "localhost",
    user = sql_username,
    password = sql_password,
    database = sql_database
        )
    
    mycursor = weather_db.cursor()
    print(mycursor)
    
    while True:
        now = datetime.now()
        dt_string = now.strftime("%Y-%m-%d %H:%M:%S")
        data, addr = client.recvfrom(1024)
        data = data.decode("utf-8")
        print("Rec. Message: %s" % data)
        data = data.split(',')
        temp = data[0]
        pressure = data[1]
        humidity = data[2]
        gasses = data[3]
        print(f'DTG: {dt_string}')
        print(f'Temp: {temp}')
        print(f'Pressure: {pressure}')
        print(f'Humidity: {humidity}')
        print(f'Gasses: {gasses}')
        
        sql = "INSERT INTO" + " " + sql_table_name + " " + "(time, temperature, pressure, humidity, gasses) VALUES (%s, %s, %s, %s, %s)"
        data = (dt_string, temp, pressure, humidity, gasses)
        mycursor.execute(sql, data)
        weather_db.commit()
        
        print(mycursor.rowcount, "Record inserted.")
