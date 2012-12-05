#pragma once


#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include "..\..\Resources\Google\Protobuf\message.h"
#include "..\Utility\FastDelegate.h"

using namespace std;

class ProtobufPipeServer
{
	// Data can not be sent through the pipe in large chunks than this because of the operating system.
	static const DWORD BUFFER_SIZE = 65535;
	struct _OverlappedWrapper;
	typedef struct {
		HANDLE hPipe;
		struct _OverlappedWrapper *readOW;
		struct _OverlappedWrapper *writeOW;
		byte inBuf[BUFFER_SIZE];
		byte outBuf[BUFFER_SIZE];
		ProtobufPipeServer *owner;
	} ClientInfo;

	typedef struct _OverlappedWrapper {
		OVERLAPPED oOverlap;
		ClientInfo *pci;
	} OverlappedWrapper;
	
	basic_string<TCHAR> name;
	void (*receiveCallback)(const byte*, const size_t);

	HANDLE hPipesMutex;
	vector<ClientInfo*> pipes;
	HANDLE hConnectThread;
	DWORD dwThreadId;
	BOOL running;

	static DWORD WINAPI ConnectionThreadEntry(LPVOID pThis) {
		return ((ProtobufPipeServer*)pThis)->ConnectionThread();
	}
	static void WINAPI ReadCallbackEntry(DWORD errorCode, DWORD numBytes, LPOVERLAPPED lpo) {
		if (errorCode) {
			if (errorCode != ERROR_BROKEN_PIPE) {
				printf("ProtobufPipeServer::ReadCallbackEntry: Received error code - %d.\n", errorCode);
			}
			return;
		}
		((OverlappedWrapper*)lpo)->pci->owner->ReadCallback(errorCode, numBytes, lpo);
	}

	DWORD WINAPI ConnectionThread();
	void ReadCallback(DWORD errorCode, DWORD numBytes, LPOVERLAPPED lpo);

	BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap, HANDLE *phPipe);

	void DestroyClientInfo(ClientInfo *pci);

public:
	/**
	* The amount of time allowed for a write operation ot complete.
	*/
	int writeTimeout;
	/**
	* The amount of time allowed to aquire the mutex.
	*/
	int waitTimeout;

	/**
	* Constructor for this object. Takes in the name to give to the pipe being created. This 
	* constructor creates the pipe and starts the thread that listens for connections to it.
	*/
	ProtobufPipeServer(basic_string<TCHAR> &name);
	/**
	* Destructor and cleanup.
	*/
	virtual ~ProtobufPipeServer(void);

	/**
	* Writes the given message to the pipe.
	*/
	BOOL WriteMessage(::google::protobuf::Message *message);

	/**
	* Set the method calback used when new messages are received.
	*/
	void SetReceiveCallback(void (*callback)(const byte*, const size_t));
};

