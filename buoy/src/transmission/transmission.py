#!/usr/bin/env python3

import json
import socket
import random

KEYFILE = "lib/key.h"
MANAGER_HOST = "manager.salmonete.com"
MANAGER_PORT = 80
MANAGER_URI = "/register.php"
IP = "127.0.0.3"

# Example data
example_data = {}
for field in [
    "ra",
    "rp",
    "rtg",
    "rtmb",
    "rteb",
    "rtsw",
    "rtb",
    "rmv",
    "rma",
    "rev",
    "rea",
    "btmb",
    "btsw",
    "bmv",
    "bma",
]:
    example_data[field] = round(random.random() * 10, 2)

# Prepare object to send
jobj = {
    "name": "Buoy",
    "version": "0.1",
    "build": "1",
    "ip": IP,
    "port": 80,
    "data": example_data,
}
jdata = json.dumps(jobj)

# Read key
with open(KEYFILE, "r") as keypointer:
    key = keypointer.read().split("\n")[0].split('"')[1]

# httpraw = f"GET http://{MANAGER_HOST}{MANAGER_URI}?name={jobj['name']}&version={jobj['version']}&build={jobj['build']}&ip={jobj['ip']}&port={jobj['port']} HTTP/1.1\r\nHost: {MANAGER_HOST}\r\nUser-Agent: Alioli Buoy v{jobj['version']}\r\nAlioli-Key: {key}\r\nAccept: text/html\r\nConnection: close\r\n\r\n"

httpraw = f"POST http://manager.salmonete.com/register.php HTTP/1.1\r\nHost: {MANAGER_HOST}\r\nUser-Agent: Alioli Buoy v{jobj['version']}\r\nAlioli-Key: {key}\r\nAccept: text/html\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: {len(jdata)}\r\n\r\n{jdata}"

# Show package ready to be sent
print("=== SEND === =======================")
print("")
print(httpraw)
print("")

# Open conneciton to server
link = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
link.connect((MANAGER_HOST, MANAGER_PORT))

# Send package
link.send(httpraw.encode())

# Get answer
print("=== RECV === =======================")
print("")
print(link.recv(4096))
print("")
