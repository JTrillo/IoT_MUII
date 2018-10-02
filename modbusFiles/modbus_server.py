from pyModbusTCP.server import ModbusServer

ip_server = "192.168.43.51" #replace it

server = ModbusServer(host=ip_server, port=502)
server.start()
