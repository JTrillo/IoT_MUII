from pyModbusTCP.server import ModbusServer

server = ModbusServer(host="localhost", port=502)
server.start()