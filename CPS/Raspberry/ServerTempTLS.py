import socket
import ssl

IP_ADDR = '192.168.43.99' #CHANGE IT
PORT = 8090 #CHANGE IT
CERTFILE = 'path/to/certfile' #CHANGE IT
KEYFILE = 'path/to/keyfile' #CHANGE IT

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((IP_ADDR, PORT))
s.listen(0)

print("Server started in {0}:{1}".format(IP_ADDR, PORT))

while True:
    print("Waiting for connections....")
    client, addr = s.accept()
    print('Client connected: ', addr)

    secure_sock = ssl.wrap_socket(client, server_side=True, certfile=CERTFILE,
                        keyfile=KEYFILE, ssl_version=ssl.PROTOCOL_TLSv1_2)

    #print(repr(secure_sock.getpeername()))
    #print(secure_sock.cipher())

    content = secure_sock.recv(32)

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
        secure_sock.sendall(response.encode("utf-8"))
    print("Closing connection...")
    secure_sock.close()
