import requests
from requests.auth import HTTPBasicAuth

# Authentication
usr = 'apikey'
pwd = '177HT6AXZNrDVi4eO30Lc6nsoNuCnE'

# Vars
dev = "1684251"
num = "+522226111169"
sms = "Hi from python 3!"

# Send SMS
response = requests.post(
'https://dashboard.hologram.io/api/1/sms/incoming',
data = {
"deviceid": dev,
"fromnumber":num,
"body": sms
},
auth = HTTPBasicAuth(usr, pwd)
)
# Logs
print(response.json())