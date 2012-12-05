#include "ProtobufPipeServer.h"
#include <tchar.h>

ProtobufPipeServer::ProtobufPipeServer(std::basic_string<TCHAR> &pipeName)
{
	hConnectThread = NULL;
	running = FALSE;
	waitTimeout = writeTimeout = 1000;

	this->name = pipeName;

	hPipesMutex = CreateMutex(NULL, false, NULL);
	if (hPipesMutex == NULL) {
		printf("ProtobufPipeServer::ProtobufPipeServer(pipe %s): failed to create mutex - %d.\n", name, GetLastError());
		throw new exception("ProtobufPipeServer::ProtobufPipeServer: failed to create mutex.");
	}

	hConnectThread = CreateThread( 
            NULL,              // no security attribute 
            0,                 // default stack size 
			ProtobufPipeServer::ConnectionThreadEntry,    // thread proc
            this,    // thread parameter 
			0,                 // not suspended 
            &dwThreadId);      // returns thread ID 

}

ProtobufPipeServer::~ProtobufPipeServer(void)
{
	running = false;

	WaitForSingleObject(hPipesMutex, waitTimeout);
	vector<ClientInfo*>::const_iterator it;
	for (it = pipes.begin(); it < pipes.end(); ++it) {
		ClientInfo *pci = *it;
		DestroyClientInfo(pci);
	}

	pipes.clear();

	ReleaseMutex(hPipesMutex);
	CloseHandle(hPipesMutex);
	CloseHandle(hConnectThread);
}

void ProtobufPipeServer::DestroyClientInfo(ClientInfo *pci) {
		DisconnectNamedPipe(pci->hPipe);
		CloseHandle(pci->hPipe);
		CloseHandle(pci->readOW->oOverlap.hEvent);
		delete pci->readOW;
		CloseHandle(pci->writeOW->oOverlap.hEvent);
		delete pci->writeOW;
		delete pci;
}

DWORD WINAPI ProtobufPipeServer::ConnectionThread() {
	HANDLE hPipe;
	HANDLE hConnectEvent; 
	OVERLAPPED oConnect; 
	ClientInfo *pClientInfo; 
	DWORD dwWait, cbRet, dwPipesWait; 
	BOOL fSuccess, fPendingIO; 

	// Create one event object for the connect operation. 

	hConnectEvent = CreateEvent( 
		NULL,    // default security attribute
		TRUE,    // manual reset event 
		TRUE,    // initial state = signaled 
		NULL);   // unnamed event object 

	if (hConnectEvent == NULL) 
	{
		printf("CreateEvent failed with %d.\n", GetLastError()); 
		return 0;
	}
	oConnect.hEvent = hConnectEvent; 

	// Call a subroutine to create one instance, and wait for 
	// the client to connect. 

	fPendingIO = CreateAndConnectInstance(&oConnect, &hPipe); 

	running = true;

	while (running) {
		dwWait = WaitForSingleObjectEx(hConnectEvent, 1000, TRUE);
		int tmpo;
		switch (dwWait) {
			case WAIT_OBJECT_0:
				ResetEvent(hConnectEvent);
				if (fPendingIO) 
				{ 
					// If an operation is pending, get the result of the 
					// connect operation. 
					fSuccess = GetOverlappedResult( 
						hPipe,     // pipe handle 
						&oConnect, // OVERLAPPED structure 
						&cbRet,    // bytes transferred 
						FALSE);    // does not wait 
					if (!fSuccess) 
					{
						printf("ProtobufPipeServer: Error occurred waiting for connection (%d)\n", GetLastError());
						CloseHandle(hPipe);
						continue;
					}
				}

				dwPipesWait = WaitForSingleObject(hPipesMutex, waitTimeout);
				if (dwPipesWait != WAIT_OBJECT_0) {
					printf("ProtobufPipeServer::ConnectionThread(pipe %s): The pipes mutex could not be aquired for writing - %d : %d.\n", name, dwPipesWait, GetLastError());
					continue;
				}
				// Allocate storage for this instance.
				pClientInfo = new ClientInfo();
				pClientInfo->owner = this;
				pClientInfo->hPipe = hPipe;
				pClientInfo->readOW = new OverlappedWrapper();
				pClientInfo->readOW->oOverlap.hEvent = CreateEvent(NULL, true, false, NULL);
				pClientInfo->readOW->oOverlap.Offset = 0;
				pClientInfo->readOW->oOverlap.OffsetHigh = 0;
				pClientInfo->readOW->pci = pClientInfo;
				pClientInfo->writeOW = new OverlappedWrapper();
				pClientInfo->writeOW->oOverlap.hEvent = CreateEvent(NULL, true, false, NULL);
				pClientInfo->writeOW->oOverlap.Offset = 0;
				pClientInfo->writeOW->oOverlap.OffsetHigh = 0;
				pClientInfo->writeOW->pci = pClientInfo;
				pipes.push_back(pClientInfo);
				if (!ReleaseMutex(hPipesMutex)) {
					printf("ProtobufPipeServer::ConnectionThread: pipe %s failed to release pipes mutex - %d\n", name, GetLastError());
				}

				ReadFileEx(pClientInfo->hPipe, &pClientInfo->inBuf, BUFFER_SIZE, (OVERLAPPED*)pClientInfo->readOW, ProtobufPipeServer::ReadCallbackEntry);

				fPendingIO = CreateAndConnectInstance(&oConnect, &hPipe);

				break;
			case WAIT_IO_COMPLETION:
				break;
			case WAIT_TIMEOUT:
				fSuccess = GetOverlappedResult( 
						hPipe,     // pipe handle 
						&oConnect, // OVERLAPPED structure 
						&cbRet,    // bytes transferred 
						FALSE);    // does not wait 
				tmpo = GetLastError();
				continue;
			default:
				printf("WaitForSingleObjectEx (%d)\n", GetLastError());
				break;
		}
		
	}

	CloseHandle(hConnectEvent);

	return 0;
}

void ProtobufPipeServer::ReadCallback(DWORD errorCode, DWORD numBytes, LPOVERLAPPED lpo) {
	if (errorCode) {
		if (errorCode != ERROR_BROKEN_PIPE) {
			printf("ProtobufPipeServer::ReadCallback(pipe %s): Received error code - %d.\n", name, errorCode);
		}
		return;
	}

	if (!running) {
		return;
	}

	ClientInfo *pci = ((OverlappedWrapper*)lpo)->pci;

	if (numBytes == 0) {
		printf("ProtobufPipeServer::ReadCallback: did not reaceive any data\n");
		return;
	}

	//printf("ProtobufPipeServer::ReadCallback: Received %d  bytes.\n", numBytes);
	if (this->receiveCallback != NULL) {
		receiveCallback(pci->inBuf, numBytes);
	}

	if (running) {
		ReadFileEx(pci->hPipe, &pci->inBuf, BUFFER_SIZE, (OVERLAPPED*)pci->readOW, ProtobufPipeServer::ReadCallbackEntry);
	}
}

BOOL ProtobufPipeServer::CreateAndConnectInstance(LPOVERLAPPED lpoOverlap, HANDLE *phPipe) 
{ 
	basic_string<TCHAR> fullName = "\\\\.\\pipe\\" + name;

	*phPipe = CreateNamedPipe(fullName.c_str(), 
		PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
		PIPE_UNLIMITED_INSTANCES,
		BUFFER_SIZE,
		BUFFER_SIZE,
		INFINITE,
		NULL);

	if (*phPipe == INVALID_HANDLE_VALUE) {
		printf("ProtobufPipeServer::CreateAndConnectInstance: Failed to create named pipe for ProtoBuf communication: %d", GetLastError());
		return FALSE;
	}

	// Call a subroutine to connect to the new client. 

	BOOL fConnected, fPendingIO = FALSE; 

	// Start an overlapped connection for this pipe instance. 
	fConnected = ConnectNamedPipe(*phPipe, lpoOverlap); 

	// Overlapped ConnectNamedPipe should return zero. 
	if (fConnected) 
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		CloseHandle(*phPipe);
		*phPipe = INVALID_HANDLE_VALUE;
		return 0;
	}

	switch (GetLastError()) 
	{ 
		// The overlapped connection in progress. 
	case ERROR_IO_PENDING: 
		fPendingIO = TRUE; 
		break; 

		// Client is already connected, so signal an event. 

	case ERROR_PIPE_CONNECTED: 
		if (PulseEvent(lpoOverlap->hEvent)) 
			break; 

		// If an error occurs during the connect operation... 
	default: 
		{
			printf("ConnectNamedPipe failed with %d.\n", GetLastError());
			return FALSE;
		}
	} 
	return fPendingIO; 
}

BOOL ProtobufPipeServer::WriteMessage(::google::protobuf::Message *message) {
	if (!running) {
		return false;
	}

	BOOL output = TRUE;
	__int32 numBytes = message->ByteSize();
	if (numBytes > BUFFER_SIZE) {
		cout << "ProtobufPipeServer::WriteMessage(pipe "<< name <<"): The size of the given message exceeds the maximum size of the write buffer."<< endl;
	}

	byte* data = new byte[numBytes];
	if (message->SerializeToArray (data,numBytes)) {
		BOOL fSuccess;
		
		DWORD dwPipesWait = WaitForSingleObject(hPipesMutex, waitTimeout);
		if (dwPipesWait != WAIT_OBJECT_0) {
			cout << "ProtobufPipeServer::WriteMessage(pipe " << name << "): The pipes mutex could not be aquired for writing - " << dwPipesWait << " : " << GetLastError() << endl;
			output = false;
		} else {
			for (int i = pipes.size()-1; i >= 0; --i) {
				ClientInfo *pci = pipes[i];

				memcpy(&pci->outBuf, data, numBytes);

				// Send message length
				WriteFile(pci->hPipe, &numBytes, sizeof(numBytes), NULL, (OVERLAPPED*)pci->writeOW);
				// Send message
				fSuccess = WriteFile(pci->hPipe, &pci->outBuf, numBytes, NULL, (OVERLAPPED*)pci->writeOW);
				if (fSuccess) {
					// Completed asynchrounously
				} else {
					DWORD bytesRead;
					DWORD err = GetLastError();
					if (err == ERROR_IO_PENDING) {
						fSuccess = GetOverlappedResult(pci->hPipe, (OVERLAPPED*)pci->writeOW, &bytesRead, true);
						if (!fSuccess) {
							cout << "ProtobufPipeServer::WriteMessage: pipe " << name << " failed to complete write operation with error " << GetLastError() << endl;
							output = FALSE;
						}
					} else {
						if (err == ERROR_BROKEN_PIPE || ERROR_NO_DATA || ERROR_PIPE_NOT_CONNECTED) {
							
							DestroyClientInfo(pci);
							pipes.erase(pipes.begin() + i);
						}
						std::cout << "ProtobufPipeServer::WriteMessage: pipe " << name << " failed to initiate write operation with error " << err << std::endl;
						output = FALSE;
					}
				}
			}

			if (!ReleaseMutex(hPipesMutex)) {
				std::cout << "ProtobufPipeServer::WriteMessage: pipe "<< name <<" failed to release pipes mutex - " << GetLastError() << std::endl;
			}
		}

	} else {
		output = FALSE;
	}
	free (data);

	return output;
}

void ProtobufPipeServer::SetReceiveCallback(void (*callback)(const byte*, const size_t)) {
	this->receiveCallback = callback;
}
