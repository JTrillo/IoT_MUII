"""from pprint import pprint
import requests
APIKEY = "e74b065c773537748aff0a1f572a24cf"
ID = "6359472"
r = requests.get('http://api.openweathermap.org/data/2.5/weather?id={ID}&APPID={APIKEY}')
pprint(r.json())"""

import socket

IP_ADDR = '192.168.43.99'
PORT = 8090

s = socket.socket()

s.bind((IP_ADDR, PORT))
s.listen(0)
print("Server started in {0}:{1}".format(IP_ADDR, PORT))

while True:
    print("Waiting for connections....")
    client, addr = s.accept()
    print('Client connected: ', addr)

    content = client.recv(32)

    if len(content) == 0:
        break
    else:
        temperature_string = content.decode("utf-8")
        print("Temperature received: " + temperature_string + "°")
        
        temperature = float(temperature_string)
        response = ''
        if temperature >= 28:
            response = 'R'
        elif temperature <=17:
            response = 'B'
        else:
            response = 'G'
                
        print("Response " + response)
        client.sendall(response.encode("utf-8"))
    print("Closing connection...")
    client.close()
