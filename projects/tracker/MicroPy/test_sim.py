import time
from sim7000 import Sim7000

sim = Sim7000(debug=1)

# Turn GPS on
if sim.setGPS(True):
    while 1:
        if sim.prepareMessage():
            print(sim.gps_data)
            break
        else:
            time.sleep(3)
