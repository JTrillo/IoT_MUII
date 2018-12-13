import paho.mqtt.client as mqtt
import ssl
import time
#To install paho-mqtt --> pip install paho-mqtt

'''
    global variables
'''
BROKER_ADDR = '192.168.43.51'
BROKER_PORT = 8883
USERNAME = "ids1"
PASSWORD = "security"
TOPIC = "node1/temperature"
CA_CERTIFICATE = "dev_cert_ca.cert.pem"
LAST_DATA = None

#Callback when client receives a CONNACK from the server
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(TOPIC)

#Callback when client receibes a pub message from the server
def on_message(client, userdata, message):
    global LAST_DATA
    print("New message from " + message.topic)
    #print(message.payload)
    decoded_payload = message.payload.decode('ascii')
    print(decoded_payload)
    
    try:
        actual = float(decoded_payload)
        print("ENTRA")
        if(LAST_DATA!=None):
            if(abs(actual-LAST_DATA)>1.0):
                print("ENTRA2")
                sendAnomaly(client)
                #client.publish(TOPIC, payload="anomaly")
        LAST_DATA = actual            
    except:
        print("ENTRA3")
        if(decoded_payload!="anomaly"):
            print("ENTRA4")
            sendAnomaly(client)
            #client.publish(TOPIC, payload="anomaly")
             
def prepareClient(client, username, password, broker, port):
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.on_message = on_message
    client.tls_set(ca_certs=CA_CERTIFICATE, certfile=None, keyfile=None, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2)
    client.tls_insecure_set(True)
    #try:
    client.connect(broker, port, 60)
    client.loop_start()
    '''except ssl.CertificateError:
        client.disconnect()
        print("Error en el certificado, no se ha podido conectar")'''

def sendAnomaly(client):
    time.sleep(10)
    client.publish(TOPIC, payload="anomaly")
    
client = mqtt.Client(client_id="ids01")
prepareClient(client, USERNAME, PASSWORD, BROKER_ADDR, BROKER_PORT)
