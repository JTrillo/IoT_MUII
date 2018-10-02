from pyModbusTCP.client import ModbusClient
import time

#toggle = True
cont = 0
c = ModbusClient(host="localhost", port=502, auto_open=True)

while True:
    if not c.is_open():
        if not c.open():
            print("unable to connect")
            
    if c.is_open():
        for addr in range(4, 7):
            is_ok = c.write_single_coil(addr, cont)
            if is_ok:
                print("bit #"+str(addr) + ": write to " + str(cont))
            else:
                print("bit #"+str(addr) + ": unable to write " + str(cont))
            cont = cont + 1
            time.sleep(2)
        
    #toggle = not toggle
    time.sleep(5)