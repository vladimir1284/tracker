from socket import socket
import requests

with socket() as server:
    server.bind(("", 8080))
    server.listen(5)
    print("[+] Server Listening")
    client, addr = server.accept()
    print(f"client connected from address {addr}")
    msg = client.recv(128).decode()
    print(msg)
    x = requests.get('http://trailerrental.pythonanywhere.com/towit/tracker_data/'+msg)
    client.sendall(x.content)