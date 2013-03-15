#!/usr/bin/python
from socket import *
import sys
serverName = '127.0.0.1'
serverPort = 12000
clientSocket = socket(AF_INET, SOCK_DGRAM)
message = ""
for line in sys.stdin:
	message += line
message = message.rstrip()
print message
clientSocket.sendto(message, (serverName, serverPort))
clientSocket.close()
