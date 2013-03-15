#!/usr/bin/python
from socket import *

serverPort = 12000
serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', serverPort))
print "The server is ready to receive"
while 1:
	message, clientAddress = serverSocket.recvfrom(4096)
	modifiedMessage = message.upper()
	print message
	serverSocket.sendto(modifiedMessage, clientAddress)
	with open("log.txt", "a+") as logFile:
		logFile.write(message + '\n')
