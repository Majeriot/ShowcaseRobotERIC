#include "FlyCapture2.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <time.h>

#include "Camera\CamDataInterface.h"

#include "opencv\cv.h"
#include "opencv\cxcore.h"
#include "opencv\highgui.h"
using namespace FlyCapture2;

// Object thar writes the image data to memory
CamDataWriter *server;

HANDLE hInstanceMutex;
HANDLE hconsoleCloseEvent;
HANDLE hMainThread;
// Number of clients to allow simultaneous access to the image data. Should 
// be at least as many as the number of client programs that you want to get 
// data from this server.
long maxSemCount = 10;

// Allow camera display feature
#define DISPLAY_ON				true

// Use shutter gain for auto-gain
#define USESHUTTERGAIN
// Set the ideal value for median for auto-gain
#define AUTOGAIN_MEDIAN_IDEAL	150
// Set the gain multiplier for auto-gain
#define AUTOGAIN_KP				0.02
// Set the gain multiplier for shutter auto-gain
#define AUTOSHUTTER_KP			0.1
// Set the gain multiplier for auto-brightness
#define AUTOBRIGHT_KP			0.005
// Set the total gain multiplier for the entire auto-gain routine
#define TOTAL_KP				1.5

// Set the image width for the Firefly camera
#define IMG_WIDTH				640
// Set the image height for the Firefly camera
#define IMG_HEIGHT				480
// Set the number of channels for the Firefly Camera
#define NUM_CHANNELS			1

// A flag indicate the program is running 
volatile bool isRunning = true;
// Memory mapped buffer for holding image canvas
void* pBuf;
// Camera placement order respect to the camera GUID embedded by the menufacturer
//int camOrder[] = {2, 1, 0};
// Flag to set if the image canvas is upside down
bool isUpsideDown = false;
// A string to be shown on the title bar of the display window
char viewWindowName[] = "CameraServer. Press q to quit";
// A boolean flag to toggle logging feature
bool logging = false;

// Image timestamp
double imageTimestamp = -1.0;

using namespace std;

//captures the closing of the console window and closes the app appropriately
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	BOOL ret = FALSE;
	SetEvent(hconsoleCloseEvent);
	isRunning = false;
	if (WaitForSingleObject(hMainThread, 4000) == WAIT_OBJECT_0) 
	{
		printf("Main thread terminated OK\n");
		ret = TRUE;
	}
	CloseHandle(hMainThread);
	ExitProcess(3);
	return ret;
}

void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf( 
        version, 
        "FlyCapture2 library version: %d.%d.%d.%d\n", 
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( timeStamp );
}

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime );
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}

int InitHandles() {
	//check there is only one instance of this isRunning (checking InstanceMutex)
	hInstanceMutex = CreateMutexA(NULL,TRUE,"InstanceMutex");   

	bool bAlreadyRunning((GetLastError() == ERROR_ALREADY_EXISTS));
    if (hInstanceMutex == NULL || bAlreadyRunning)
    {
    	if(hInstanceMutex)
    	{
    		::ReleaseMutex(hInstanceMutex);
    		::CloseHandle(hInstanceMutex);
    	}
    	return -1;
    }

	////now try to create the camera mutex

	//create a global event to let clients know when this closes
	hconsoleCloseEvent = CreateEventA(NULL, TRUE, FALSE, "camMMF_quit");

	//duplicate the handle to this thread so we can detect when the app closes
	if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),  &hMainThread, 0, FALSE, DUPLICATE_SAME_ACCESS)) 
		printf("could not duplicate app thread handle\n");

	//setup the handler for when the console is closed
	SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

	return 0;
}

int InitCamera(PGRGuid guid, Camera* pCamera)
{
	Error error;

	error = pCamera->Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		getchar();
		return -1;
	}
	// Get the camera information
	CameraInfo camInfo;
	error = pCamera->GetCameraInfo(&camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		getchar();
		return -1;
	}
	PrintCameraInfo(&camInfo);  

	// Set all cameras to a specific mode and frame rate so they
    // can be synchronized
    error = pCamera->SetVideoModeAndFrameRate( 
        VIDEOMODE_640x480Y8, 
		FRAMERATE_60 );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        printf( "Error setting video mode. \n");
        getchar();
        return -1;
    }
	FC2Config config;
	error = pCamera->GetConfiguration(&config);
	if (error != PGRERROR_OK)
    {
        PrintError( error );
        printf( 
            "Error getting camera config. \n"
            "Press Enter to exit. \n");
        getchar();
        return -1;
    }
	config.grabMode = DROP_FRAMES;
	error = pCamera->SetConfiguration(&config);
	if (error != PGRERROR_OK)
    {
        PrintError( error );
        printf( 
            "Error setting camera config. \n"
            "Press Enter to exit. \n");
        getchar();
        return -1;
    }

	error = pCamera->StartCapture();
	if (error != PGRERROR_OK)
    {
        PrintError( error );
		getchar();
        return -1;
    }

	return 0;
}

unsigned int cycleTimeSeconds = 0;
bool incrementCycleTimeSeconds = false;
double GetCycleTimestampFromImage(Image * imagePtr)
{
	TimeStamp timestamp = imagePtr->GetTimeStamp();
	if (timestamp.cycleSeconds == 0 && incrementCycleTimeSeconds)
	{
		cycleTimeSeconds += 128;
		incrementCycleTimeSeconds = false;
	}
	else if (timestamp.cycleSeconds == 127)
		incrementCycleTimeSeconds = true;
	return cycleTimeSeconds + timestamp.cycleSeconds + (((double)timestamp.cycleCount+((double)timestamp.cycleOffset/3072.0))/8000.0);
}

void PasteToCanvas(IplImage* canvas, Image rawImage, int camIndex)
{
	IplImage* raw = cvCreateImage(cvSize(rawImage.GetCols(), rawImage.GetRows()), IPL_DEPTH_8U, 1);
	memcpy(raw->imageData, rawImage.GetData(), IMG_WIDTH*IMG_HEIGHT);
	// Set the image ROI to display the current image
    cvSetImageROI(canvas, cvRect((int)(0+camIndex*IMG_WIDTH), 0, IMG_WIDTH, IMG_HEIGHT));

    // Resize the input image and copy the it to the Single Big Image
    cvResize(raw, canvas);
    // Reset the ROI in order to display the next image
    cvResetImageROI(canvas);
	cvReleaseImage( &raw);
}

char* logDir = "C:\\Camera_Log";
int main(int argc, char** argv)
{   
	// Initialize synchronization and event handles
	int anotherInstanceRunning = InitHandles();
	if (anotherInstanceRunning != 0) {
		printf("Another instance is already running\n");
		return anotherInstanceRunning;
	}
	unsigned int detectedCameras = 0;
	int numCameras = 0;
	int wantedNumCameras = 1;
	int wantedCameraId[3] = {0, 1, 2};
	if (argc > 1) {
		int order = atoi(argv[1]);
		wantedCameraId[0] = (order%1000)/100;
		wantedCameraId[1] = (order%100)/10;
		wantedCameraId[2] = (order%10);
	}
	if (argc > 2) {
		logDir = argv[2];
	}
	if (argc > 3) {
		wantedNumCameras = atoi(argv[3]);
	}
	if (argc > 4) {
		wantedCameraId[0] = atoi(argv[4]);
	}
	if (argc > 5) {
		wantedCameraId[1] = atoi(argv[5]);
	}
	if (argc > 6) {
		wantedCameraId[2] = atoi(argv[6]);
	}

    PrintBuildInfo();

	// Define IplImages
	IplImage* img;		// image canvas IplImage to be put into memory
	IplImage* imgrz;	// resized image 
	IplImage* imrgb;

    Error error;

	BusManager busMgr;
    
	error = busMgr.GetNumOfCameras(&detectedCameras);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		getchar();
		return -1;
	}
	if (wantedNumCameras > detectedCameras)
	{
		printf("Number of cameras specified is greater than number of cameras detected!\n");
		getchar();
		return -1;
	}
	else
	{
		printf( "Number of cameras detected: %u\n", detectedCameras );
		numCameras = wantedNumCameras;
		printf("Using %u cameras\n", numCameras);
	}

	int canvasWidth = IMG_WIDTH*numCameras;
	int canvasHeight = IMG_HEIGHT*((int)ceil(numCameras/4.f));
	img = cvCreateImage (cvSize(canvasWidth,canvasHeight),IPL_DEPTH_8U,1);
	imgrz = cvCreateImage (cvSize(canvasWidth/2,canvasHeight/2),IPL_DEPTH_8U,1);
	Camera** ppCameras = new Camera*[numCameras];
	
	// Create inter-proc comm objects
	server = new CamDataWriter(canvasHeight*canvasWidth*NUM_CHANNELS);

	isRunning=true;

	cvNamedWindow (viewWindowName);
	cvResizeWindow(viewWindowName,200,1);
	
	bool display_image=0;

	for (int i = 0; i < numCameras; ++i)
	{
		PGRGuid guid;
		error = busMgr.GetCameraFromIndex(wantedCameraId[i], &guid);
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			getchar();
			return -1;
		}
		ppCameras[i] = new Camera();
		if (InitCamera(guid, ppCameras[i]) != 0)
			return -1;
	}

	Image rawImage;  
	int imgCnt = 0;

	int* lastMedian = new int[numCameras];
	char filename[300];
	double cycleTime = 0;
	double timeToPrint = 0;
	while(isRunning)
	{
		#pragma region Display and key handling
		switch(cvWaitKey (1)){
			case 'q':
				isRunning=false;
				break;
			case 'l':
				logging = !logging;
				if (logging) {
					printf("Logging enabled.\n");
				} else {
					printf("Logging disabled.\n");
				}
				break;
			case 0x20:	//space-bar
				if(!display_image){
					display_image = !display_image;
					if(imgrz->width > 0 && imgrz->height > 0)
						cvResizeWindow(viewWindowName,imgrz->width, imgrz->height);
				}
				else{
					display_image = !display_image;
					cvResizeWindow(viewWindowName,200, 1);
				}				
				break;
			default:
				break;
		}
		#pragma endregion
	
		#pragma region Acquire images from cameras
		
		for (int i = 0; i < numCameras; ++i)
		{
			if (imgCnt == 0)
			{
				lastMedian[i] = 0;
			}
			
			// Retrieve an image
			error = ppCameras[i]->RetrieveBuffer( &rawImage );
			if (error != PGRERROR_OK)
			{
				PrintError( error );
				return -1;
			}

			imageTimestamp = GetCycleTimestampFromImage(&rawImage);
			/*printf( "Grabbed image from Cam %d - Frame %d - TimeStamp [%d %d %d]\n", i, 
				imgCnt,
				timestamp.cycleCount,
				timestamp.cycleOffset, 
				timestamp.cycleSeconds,
				timestamp.microSeconds); */

			PasteToCanvas(img, rawImage, i);
		}

		if (isUpsideDown)	cvFlip (img,img,-1);

		#pragma endregion

		// Write Image into memory
		server->WriteNextImage(img->imageData, canvasWidth*canvasHeight*NUM_CHANNELS, imageTimestamp);
		
		imgCnt++;
		if(DISPLAY_ON && display_image)
		{
			if(imgCnt%3!=0) continue; 
			cvResize(img,imgrz,1);
			cvShowImage(viewWindowName,imgrz );
		}
	}

	// Clean up handles
	delete server;
	CloseHandle(hconsoleCloseEvent);
	for (int i = 0; i < numCameras; i++ )
    {
        ppCameras[i]->StopCapture();
        ppCameras[i]->Disconnect();
        delete ppCameras[i];
    }

    delete [] ppCameras;
	delete [] lastMedian;

	cvDestroyAllWindows();

    printf( "Done! Press Enter to exit...\n" );
    getchar();

    return 0;
}
