from re import M
import time
from sim7000 import Sim7000

sim = Sim7000(debug=1)

# # Turn GPS on
# sim.setGPS(True)
# while 1:
#     if sim.prepareMessage():
#         print(sim.gps_data)
#     time.sleep(3)

# Turn LTE on
sim.setLTE(True)

msg = ""
sim.uploadData(msg)

sim.setLTE(False)
