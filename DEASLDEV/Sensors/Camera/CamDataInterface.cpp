#include "CamDataInterface.h"

#include <stdio.h>


bool AcquireSemaphores(HANDLE hSem, int num) {
	for (int i = 0; i < num;) {
		DWORD result = WaitForSingleObject(hSem, 1000);
		if (result == WAIT_OBJECT_0) {
			++i;
		} else {
			printf("Could not acquire semaphore lock. Trying again.\n");
			return false;
		}
	}

	return true;
}

void ReleaseSemaphores(HANDLE hSem, long numToRelease) {
	long prevCount;
	if(!ReleaseSemaphore(hSem, numToRelease, &prevCount)) {
		printf("Error Code1: %d\n", GetLastError());
		ReleaseSemaphore(hSem, prevCount, NULL);
		
		printf("Error Code2: %d\n", GetLastError());
	}
}


const char* CamDataWriter::DEFAULT_CAM_NAME = "Global\\CamMappingObject";
const char* CamDataWriter::DEFAULT_SEMAPHORE_NAME = "CSSemaphore";
const char* CamDataWriter::DEFAULT_MUTEX_NAME = "CSSignalMutex";

void CamDataReader::init(const char* cameraName, const char* semaphoreName, const char* mutexName) {
	//we use this mutex to know when there are new images...
	hSignalMutex = OpenMutex(SYNCHRONIZE, false, mutexName);
	if (hSignalMutex == NULL) {
		printf("OpenMutex error: %d\n", GetLastError());
	}
	hAccessSem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE , false, semaphoreName);
	if (hAccessSem == NULL) {
		printf("OpenSemaphore error: %d\n", GetLastError());
	}

	hMapFile = NULL;
	while(hMapFile == NULL)
	{
		hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, cameraName);
		if (hMapFile == NULL) 
		{
			printf("Could not access file mapping object (%d).\n",GetLastError());
			Sleep(1000);
		}		
	}	
	
	pBuf = MapViewOfFile(this->hMapFile,   // handle to map object
		FILE_MAP_READ, // read permission
		0,                   
		0,                   
		0);            //goto max size

	// Check to make sure we mapped the image data.
	if (pBuf == NULL) 
	{ 
		printf("Could not map view of file (%d).\n", GetLastError());
	}

}

CamDataReader::~CamDataReader() {
	UnmapViewOfFile(pBuf);
	ReleaseMutex(this->hSignalMutex);
	CloseHandle(this->hSignalMutex);
	ReleaseSemaphore(this->hAccessSem, 1, NULL);
	CloseHandle(this->hAccessSem);
	CloseHandle(this->hMapFile);
}

bool CamDataReader::ReadNextImage(char* destImg, size_t imageSize, double* timestamp) {
	// Attempt to gain signalling mutex to check if a new image has arrived. A 
	// mutex is used instead of an event because the event methods don't provide 
	// the guarantees I wanted about releasing waiting threads.
	if (WaitForSingleObject(hSignalMutex, 5000)!=WAIT_OBJECT_0)
	{
		printf("Error code: %d\n", GetLastError());
		printf("Warning: Did not receive signal mutex after read in 5 seconds...\n");
		Sleep(1000);
		return false;
	}
	// We immediately release the mutex because we are using it as a signal, not to block access to a variable.
	ReleaseMutex(hSignalMutex);

	// Ensure that we have enough resources for this client to read the image data.
	if (WaitForSingleObject(hAccessSem,5000)!=WAIT_OBJECT_0)
	{
		printf("Warning: Did not receive image semaphore in 5 seconds.");
		Sleep(1000);
		return false;
	}

	//void* pBuf = MapViewOfFile(this->hMapFile,   // handle to map object
	//	FILE_MAP_READ, // read permission
	//	0,                   
	//	0,                   
	//	0);            //goto max size

	// Check to make sure we mapped the image data.
	if (pBuf == NULL) 
	{ 
		printf("Could not map view of file (%d).\n", GetLastError()); 
		Sleep(1000);
		ReleaseSemaphore(hAccessSem, 1, NULL);
		return false;
	}

	// Copy data to local variables.
	memcpy(destImg,pBuf,imageSize);
	memcpy(timestamp,(char*)pBuf + imageSize,sizeof(double));

	//// Release shared data.
	//UnmapViewOfFile(pBuf);
	// Release semaphore
	ReleaseSemaphore(hAccessSem, 1, NULL);

	// New image was successfully copied
	return true;
}

void CamDataWriter::init(const char* cameraName, const char* semaphoreName, const char* mutexName, size_t imageSize) {
	// Create inter-proc comm objects
	hSignalMutex = CreateMutex(NULL, false, mutexName);
	if (hSignalMutex == NULL) {
		printf("Error occurred while creating write lock mutex: %d\n", GetLastError());
	}
	hAccessSem = CreateSemaphore(NULL, MAX_SEM_COUNT, MAX_SEM_COUNT, semaphoreName);
	if (hAccessSem == NULL) {
		printf("Error occurred while creating semaphore: %d\n", GetLastError());
	}

	// Set up shared memory
	hMapFile = CreateFileMappingA(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security 
		PAGE_READWRITE,          // read/write access
		0,                       // max. object size 
		imageSize + sizeof(double), // buffer size  
		cameraName);                 // name of mapping object

	if (hMapFile == NULL) 
	{ 
		printf("Could not create file mapping object (%d).\n", 
			GetLastError());

	}
	pBuf = MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,                   
		0,                   
		0);            //goto max size

	if (pBuf == NULL) 
	{ 
		printf("Could not map view of file (%d).\n", GetLastError()); 
	}
}

CamDataWriter::~CamDataWriter() {
	AcquireSemaphores(hAccessSem, MAX_SEM_COUNT);
	UnmapViewOfFile(pBuf);
	ReleaseMutex(this->hSignalMutex);
	CloseHandle(this->hSignalMutex);
	ReleaseSemaphore(this->hAccessSem, 1, NULL);
	CloseHandle(this->hAccessSem);
	CloseHandle(this->hMapFile);
}

bool CamDataWriter::WriteNextImage(char* srcImg, size_t srcSize, double timestamp) {
	// Aquire all semaphores to keep any clients from accessing the image data - close second door
	if (!AcquireSemaphores(hAccessSem, MAX_SEM_COUNT)) {
			ReleaseSemaphores(hAccessSem, MAX_SEM_COUNT);
			printf("Failed to aquire all semaphores.\n");
			return false;
		}
		// Release signalling mutex to tell waiting clients that a new image will soon be ready for reading - open first door
		ReleaseMutex(hSignalMutex);
		// Copy data to shared memory
		CopyMemory(pBuf, srcImg, srcSize);		
		CopyMemory((char*)pBuf+(srcSize), &timestamp,sizeof(double));

		// Aquire signalling mutex to keep clients from reading the same image twice - close first door
		DWORD result = WaitForSingleObject(hSignalMutex, 1000);
		if (result != WAIT_OBJECT_0) {
			printf("Could not aquire write lock mutex in 5 seconds...\n");
			return true;
		}
		// Release all semaphores to allow clients to access new image - open second door
		ReleaseSemaphores(hAccessSem, MAX_SEM_COUNT);

		return true;
}