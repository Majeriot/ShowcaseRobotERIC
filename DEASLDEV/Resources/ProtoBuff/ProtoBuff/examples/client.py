#!/usr/bin/env python
from socket import *
from numpy import *
import ltlmopMsg_pb2



#??# Set up socket for communication from executor
print "(GUI) Starting socket for communication from controller"
#host = gethostbyname("localhost")
#host='localhost'
host='0.0.0.0'
buf = 1024
portFrom =8844

addrFrom = (host,portFrom)

UDPSockFrom = socket(AF_INET,SOCK_DGRAM)
UDPSockFrom.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
UDPSockFrom.bind(addrFrom)

count = 1
while count>0:
    # Wait for and receive a message from the controller
    ltlmop_msg = ltlmopMsg_pb2.InitMessage()
    
    m_input,addr = UDPSockFrom.recvfrom(buf)

    input_msg = ltlmop_msg.ParseFromString(m_input.strip())
    print "Got input!"
    print "length",len(input_msg)
    print input
    count = count-1
    #robotPose.ParseFromString(input)
    #print robotPose.x,robotPose.y,robotPose.z
    
    #c= eval("array(" + input.replace('\n','').replace('] [', '],[').replace(' ', ',') + ")")


    #print "received!"
    #print c
    #print "the size is ",c.shape
    #print c[0,0]
