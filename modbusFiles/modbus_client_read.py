from pyModbusTCP.client import ModbusClient
import time

ip_host = "192.168.43.51"
c = ModbusClient(host=ip_host, port=502, auto_open=True)

while True:
    if not c.is_open():
        if not c.open():
            print("unable to connect to " + ip_host)
            
    if c.is_open():
        """regs = c.read_holding_registers(0, 10)
        if regs:
            print("reg ad #0 to 9: "+str(regs))"""
        bits = c.read_coils(0,10)
        if bits:
            print("bit ad #0 to 9: "+str(bits)) 
            
    time.sleep(2)
