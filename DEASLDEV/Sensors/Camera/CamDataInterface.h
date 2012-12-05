#pragma once

#include <windows.h>



class CamDataWriter {
	// Number of clients to allow simultaneous access to the image data. Should 
	// be at least as many as the number of client programs that you want to get 
	// data from this server.
	static const long MAX_SEM_COUNT = 10;
	/**
	* OS handle to the shared memory the image is stored in.
	*/
	HANDLE hMapFile;
	/**
	* OS handle to the mutex used to signal clients when a new image is available.
	*/
	HANDLE hSignalMutex;
	/**
	* OS handle to the semaphore used to keep the server from writing new image data when clients are current reading data.
	*/
	HANDLE hAccessSem;

	void *pBuf;

	/**
	* Method called by the constructors which creates the synchronization and 
	* data objects used for communication between the camera server and its clients.
    * @param camName Name of the shared memory object the image data is stored in.
    * @param semaphoreName Name of the semaphore used to keep this writer from writing new image data when clients are current reading data.
    * @param mutexName Name of the mutex used to signal clients when a new image is available.
	* @param imageSize Number of bytes needed to store the image.
	*/
	void init(const char* camName, const char *semaphoreName, const char* mutexName, size_t imageSize);

public:
	/**
	 * Default name of the shared memory region that contains the image data.
	 */
	static const char* DEFAULT_CAM_NAME;
	/**
	 * Default name of the semaphore used to delay the server from writing data to shared memory until no clients are reading the data.
	 */
	static const char* DEFAULT_SEMAPHORE_NAME;
	/**
	 * Default name of the mutex object used to signal clients that a new image is ready.
	 */
	static const char* DEFAULT_MUTEX_NAME;

	/**
	 * Default constructor for the writer. Uses the default names for the synchronization objects.
	 * @param imageSize Number of bytes needed to store the image.
	 */
	CamDataWriter(size_t imageSize) {
		init(DEFAULT_CAM_NAME, DEFAULT_SEMAPHORE_NAME, DEFAULT_MUTEX_NAME, imageSize);
	}

	/**
	 * Creates this object using the given names of the synchronization objects.
	 * @param camName Name of the shared memory object the image data is stored in.
     * @param semaphoreName Name of the semaphore used to keep this writer from writing new image data when clients are current reading data.
     * @param mutexName Name of the mutex used to signal clients when a new image is available.
	 * @param imageSize Number of bytes needed to store the image.
	 */
	CamDataWriter(const char* camName, const char *semaphoreName, const char* mutexName, size_t imageSize) {
		init(camName, semaphoreName, mutexName, imageSize);
	}
	/**
	 * Destructor for this object.
	 */
	~CamDataWriter();

	/**
	 * Write the next image from to shared memory. This method will wait for up to 5 seconds 
	 * to try to wirte the image. If it fails, this method will return false.
	 * @return Whether a new image was successfully written.
	 */
	bool WriteNextImage(char* srcImg, size_t dstSize, double timestamp);
};

/**
 * A class used to access image put into shared memory by the camera servers. The 
 * purpose of this class is to deal with the synchronization details used to ensure 
 * that clients only read each image once, and don't read the image while it is being 
 * written. 
 */
class CamDataReader {
	
	/**
	* OS handle to the shared memory the image is stored in.
	*/
	HANDLE hMapFile;
	/**
	* OS handle to the mutex used to signal clients when a new image is available.
	*/
	HANDLE hSignalMutex;
	/**
	* OS handle to the semaphore used to keep the server from writing new image data when clients are current reading data.
	*/
	HANDLE hAccessSem;

	void *pBuf;

	/**
	* Method called by the constructors which gets access to the synchronization and 
	* data objects used for communication between the camera server and its clients.
    * @param camName Name of the shared memory object the image data is stored in.
    * @param semaphoreName Name of the semaphore used to keep the server from writing new image data when clients are current reading data.
    * @param mutexName Name of the mutex used to signal clients when a new image is available.
	*/
	void init(const char* camName, const char *semaphoreName, const char* mutexName);

public:
	/**
	 * Default constructor for the reader. Uses the default names for the synchronization objects.
	 */
	CamDataReader() {
		init(CamDataWriter::DEFAULT_CAM_NAME, CamDataWriter::DEFAULT_SEMAPHORE_NAME, CamDataWriter::DEFAULT_MUTEX_NAME);
	}

	/**
	 * Creates this object using the given names of the synchronization objects.
	 * @param camName Name of the shared memory object the image data is stored in.
     * @param semaphoreName Name of the semaphore used to keep the server from writing new image data when clients are current reading data.
     * @param mutexName Name of the mutex used to signal clients when a new image is available.
	 */
	CamDataReader(const char* camName, const char *semaphoreName, const char* mutexName) {
		init(camName, semaphoreName, mutexName);
	}
	/**
	 * Destructor for this object.
	 */
	~CamDataReader();

	/**
	 * Retrieves the next new image from the server. This method will wait for 5 seconds 
	 * for a new image to arrive. If it fails, this method will return false.
	 * @return Whether a new image was successfully retrieved.
	 */
	bool ReadNextImage(char* destImg, size_t dstSize, double* timestamp);
};
