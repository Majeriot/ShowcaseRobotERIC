#include "ProtoBuffMulticast.h"

#ifdef __cplusplus_cli
#pragma unmanaged
#endif


//RECIEVER-----------------------------------------------------------------------------------------------------------
void ProtoBuffReceiver::SetCallback(msg_handler handler, void* arg)
{
	cbk = handler;
	cbk_arg = arg;
}


ProtoBuffReceiver::ProtoBuffReceiver() 
{
	udp_params params = udp_params();
	params.remote_ip = inet_addr(UDP_OOI_DETECTOR_MULTICAST_ADDR);
	params.local_port = UDP_OOI_DETECTOR_PORT;
	params.reuse_addr = 1;
	conn = new udp_connection (params);	
	conn->set_callback (MakeDelegate(this,&ProtoBuffReceiver::UDPCallback),conn);
	printf("ProtoBuffReceiver RX Interface Initialized. %s:%d\r\n",UDP_OOI_DETECTOR_MULTICAST_ADDR,UDP_OOI_DETECTOR_PORT);
}

ProtoBuffReceiver::~ProtoBuffReceiver ()
{
	delete conn;
	printf("ProtoBuffReceiver Shutdown...\r\n");
}


void ProtoBuffReceiver::UDPCallback(udp_message& msg, udp_connection* conn, void* arg)
{ 	
	ARTagMessage example2;
	example2.ParseFromArray (msg.data,msg.len);
	if (!(cbk.empty()))	
		cbk(example2, this, cbk_arg);	
}


//TRANSMITTER----------------------------------------------------------------------------------------------------

ProtoBuffSender::ProtoBuffSender()
{
	udp_params params = udp_params();
	params.remote_ip = inet_addr(UDP_OOI_DETECTOR_TARGET_MULTICAST_ADDR);	
	params.local_port = 0;
	
//bind to the 192.168.1.x subnet
	unsigned int outIP=0;	
	if (find_subnet_adapter(108736,outIP))
		printf("Bound OK\n");
	params.local_ip = outIP;
	params.local_port = 0;
	params.multicast_loopback = true;
	params.multicast_ttl=10;
	conn = new udp_connection (params);		
	sequenceNumber=0;
	printf("ProtoBuffSender TX Interface Initialized. %s:%d\r\n",UDP_OOI_DETECTOR_TARGET_MULTICAST_ADDR,UDP_OOI_DETECTOR_TARGET_PORT);
}


ProtoBuffSender::~ProtoBuffSender()
{
	printf("ProtoBuffSender Shutdown...\r\n");
}

void ProtoBuffSender::Send(ARTagMessage *m) 
{		
	int numBytes = m->ByteSize();
	unsigned char* hazaa = (unsigned char*)malloc (numBytes);
	if (m->SerializeToArray (hazaa,numBytes))
		conn->send_message(hazaa,numBytes,inet_addr(UDP_OOI_DETECTOR_TARGET_MULTICAST_ADDR),UDP_OOI_DETECTOR_TARGET_PORT);
	free (hazaa);
}

