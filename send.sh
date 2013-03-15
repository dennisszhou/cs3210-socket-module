#!/bin/bash
while :
do
	cat /proc/monitorlog | ./UDPClient2.py
	sleep 1
done
