#pragma once

#include "..\..\Network\udp_connection.h"
#include "..\..\Network\net_utility.h"
#include "..\Utility\FastDelegate.h"
#include "..\Proto\ARTagOutput.pb.h"

#define UDP_OOI_DETECTOR_PORT 30004
#define UDP_OOI_DETECTOR_MULTICAST_ADDR "239.132.2.4"
#define UDP_OOI_DETECTOR_TARGET_PORT 30033
#define UDP_OOI_DETECTOR_TARGET_MULTICAST_ADDR "239.132.2.33"

#ifdef __cplusplus_cli
#pragma managed(push,off)
#endif

using namespace ARTag;
using namespace std;

class ProtoBuffReceiver;
class ProtoBuffSender;


typedef FastDelegate3<ARTagMessage, ProtoBuffReceiver*, void*>  msg_handler;


class ProtoBuffReceiver
{
private:
	udp_connection *conn;		
	void UDPCallback(udp_message& msg, udp_connection* conn, void* arg);
	msg_handler cbk;
	void* cbk_arg;
	
public:
	ProtoBuffReceiver(void);
	~ProtoBuffReceiver(void);
	
	void SetCallback(msg_handler handler, void* arg);	
	int packetCount; int dropCount; int sequenceNumber;
	
};

class ProtoBuffSender
{
private:
	udp_connection *conn;
	void UDPCallback(udp_message msg, udp_connection* conn, void* arg);
	int sequenceNumber;
public:
	ProtoBuffSender(void);
	~ProtoBuffSender(void);

	void Send(ARTagMessage* msg);	
};

#ifdef __cplusplus_cli
#pragma managed(pop)
#endif