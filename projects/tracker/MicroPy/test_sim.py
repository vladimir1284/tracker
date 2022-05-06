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

msg = "865235030717330,3,0,0,40.73432,-111.49773,0,152,0,4168"
while 1:
    if sim.uploadData(msg):
        break
    time.sleep(3)

sim.setLTE(False)
