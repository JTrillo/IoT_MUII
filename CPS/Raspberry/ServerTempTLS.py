import socket
import ssl
import sys

IP_ADDR = '192.168.43.65' #CHANGE IT
PORT = 8090 #CHANGE IT
CERTFILE = '192.168.43.65.cert.pem' #CHANGE IT
KEYFILE = '192.168.43.65.key.pem' #CHANGE IT
CACERT = 'dev_cert_ca.cert.pem' #CHANGE IT

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
                                  cert_reqs=ssl.CERT_REQUIRED, keyfile=KEYFILE,
                                  ca_certs=CACERT, ssl_version=ssl.PROTOCOL_TLSv1_2)
    #print(repr(secure_sock))
    #print("Certificado cliente:", secure_sock.getpeercert())
    #print(repr(secure_sock.getpeername()))
    #print(secure_sock.cipher())

    try:
        ssl.match_hostname(secure_sock.getpeercert(), 'client')
        #ssl.match_hostname(secure_sock.getpeercert(), 'client2')

        content = secure_sock.recv(1024)
        
        if len(content) == 0:
            break
        else:
            temperature_string = content.decode("utf-8")
            print(temperature_string)
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
            secure_sock.send(response.encode("utf-8"))
            #print(response.encode("utf-8").hex())
    except ssl.CertificateError:
        print("Common name not valid")
        pass
    
    print("Closing connection...")
    secure_sock.close()
    print("")
