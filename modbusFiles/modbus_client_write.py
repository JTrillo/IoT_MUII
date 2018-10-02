from pyModbusTCP.client import ModbusClient
import time

list = [1, 3, 5, 8]
toggle = True
ip_host = "192.168.43.51"
c = ModbusClient(host=ip_host, port=502, auto_open=True)

while True:
    if not c.is_open():
        if not c.open():
            print("unable to connect")
            
    if c.is_open():
        for addr in list:
            is_ok = c.write_single_coil(addr, toggle)
            if is_ok:
                print("bit #"+str(addr) + ": write to " + str(toggle))
            else:
                print("bit #"+str(addr) + ": unable to write " + str(toggle))
            time.sleep(2)
        
    toggle = not toggle
    time.sleep(5)
