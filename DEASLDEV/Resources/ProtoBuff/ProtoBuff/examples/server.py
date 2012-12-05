#!/usr/bin/env pythond
from socket import *
import ltlmopMsg_pb2
import sys
#??# Set up socket for communication to executor
print "(GUI) Starting socket for communication to controller"
#host = '0.0.0.0'
host  = "localhost"
portTo = 8844
buf = 10240
addrTo = (host,portTo)
UDPSockTo = socket(AF_INET,SOCK_DGRAM)



from numpy import *
ltlmop_msg = ltlmopMsg_pb2.InitMessage()
msg.id = 10
msg.sensor = 1
ltlmop_msg.add(msg)
#a = array([[1,2,3],[4,5,6]])

#b = str(a)

UDPSockTo.sendto(ltlmop_msg.SerializeToString(),addrTo)
