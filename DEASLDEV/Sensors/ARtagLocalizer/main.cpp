//ty244 - November 3, 2010
//Find ARtag, Identify its ID and localize it 
//then send to C# part using protobuff

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif			

#define UDPIMGWIDTH 360//480
#define UDPIMGHEIGHT 90//120

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include "..\Camera\udpCameraBroadcaster.h"
#include "..\Camera\actuationInterface.h"

#include "opencv\cv.h"
#include "opencv\highgui.h"

#include "ARtagLocalizer.h"
#include "..\ProtoBufPipes\ProtobufPipeServer.h"

#include "..\Camera\CamDataInterface.h"

using namespace std;

#define BROADCAST

int WIDTH = 640;
int HEIGHT = 480;
int CHANNELS = 3;

string camName;

//camera configuration
int broadcastmod=0;
int robotID;

bool usePipes = false;
char *pipeName = "ARTagLocalizer";
ProtobufPipeServer *pPipe;

bool running=true;
bool hidedisplay = true;
bool displayhidden = false;
bool initfailed = true;
bool logcam = false;
int frameNum=0;
int calibframeNum = 0;
double timestamp = 0;
double oldtimestamp = -1;
IplImage* img;
IplImage* dispImg;
IplImage* resize;
IplImage* resizeBW;
IplImage* arimg;
IplImage* colorImg;
unsigned char* rawimg;
char tmpFilename[200];
int imgNum=0;
CvFont font = cvFont(.75,1);
ProtoBuffSender* sender;
vector<string> camParamFiles(1);

// Running time measurement
clock_t t1,t2;

using namespace std;

vector<vector<ARtag>> alltags(1);

ActInterfaceReceiver* actuation; 
double actTimestamp = 0;
void ActCallback(double timestamp, ActInterfaceReceiver* rx, void* data)
{
	actTimestamp = timestamp;
}

void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition( hStdOut, homeCoords );
}

int GetRobotID()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return 255;
	}

	char ac[80];
	char ac2[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) 
	{
		cerr << "Error " << WSAGetLastError() <<	" when getting local host name." << endl;
		return 0;
	}
	cout << "Host name is " << ac << "." << endl;
	int ret=0;
	sscanf(ac,"spider%s",ac2); 
	//	sscanf(ac,"spider%d",ret); 
	ret = atoi(ac2);
	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) 
	{
		cerr << "Yow! Bad host lookup." << endl; return 0;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) 
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
	}

	WSACleanup();
	return ret;
}

void InitCommon ()
{	
	img = cvCreateImage (cvSize(WIDTH,HEIGHT),IPL_DEPTH_8U,CHANNELS);
	resize = cvCreateImage (cvSize(WIDTH,HEIGHT),IPL_DEPTH_8U,3);
	resizeBW = cvCreateImage(cvSize(WIDTH,HEIGHT),IPL_DEPTH_8U,1);
	arimg = cvCreateImage (cvSize(640,480),IPL_DEPTH_8U,1);
	if (WIDTH > 640) {
		colorImg = cvCreateImage(cvSize(UDPIMGWIDTH, UDPIMGHEIGHT), IPL_DEPTH_8U, 3);	//480x120
		dispImg = cvCreateImage(cvSize(WIDTH/2, HEIGHT/2), IPL_DEPTH_8U, 3);
	}
	else {
		colorImg = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3);
		dispImg = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
	}

	if (initARtagPose(640, 480, 100.0, camParamFiles) == 0)
	{
		initfailed = false;
	}

	rawimg = new unsigned char[640*480];
	if (usePipes) {
		basic_string<TCHAR> nameStr(pipeName);
		pPipe = new ProtobufPipeServer(nameStr);
	} else {
		sender = new ProtoBuffSender();
	}

	actuation = new ActInterfaceReceiver();
	actuation->SetFeedbackCallback (ActCallback, NULL);

	frameNum=0;
	running=true;		
}

void ConvertToRawImage (IplImage* img, unsigned char* rawimg)
{
	for (int i=0; i<640*480; i++)
		rawimg[i] = *(unsigned char*)(img->imageData + i);
}

void DisplayMsgInConsole()
{
	ClearScreen();
	if (initfailed)
		printf("Init Failed! Make sure the calibration filepath was specified.\n");
	else
		for (int j = 0; j < camParamFiles.size(); j++)
		{
			string camParamFilePath = camParamFiles[j];
			printf("Calib-file %d: %s.\n", j, camParamFilePath.c_str());
		}

	for (int i = 0; i < alltags.size(); ++i) {
		vector<ARtag> theTags = alltags[i];
		printf("Cam ID: %i\n", i);
		for (int j = 0; j < theTags.size(); ++j) {

			ARtag tag = theTags[j];
			cv::Mat pose = tag.getPose();
			float x = pose.at<float>(0,3) / 1000.0;
			float y = pose.at<float>(1,3) / 1000.0;
			float z = pose.at<float>(2,3) / 1000.0;
			float yaw = atan2(pose.at<float>(1,0), pose.at<float>(0,0));
			printf("\tTag Id: %d\n", tag.getId());
			printf("x: %.2f \t y: %.2f \t z: %.2f \t yaw: %.2f\n", x,y,z,yaw);
			printf("\n");
		}
	}
}

double lastTS=0;
void RunCamera(double timestamp)
{	
	cvCvtColor (resize,resizeBW,CV_BGR2GRAY);
	for (int camNum = 0; camNum < camParamFiles.size(); ++camNum) {
		vector<ARtag> tags;
		cvSetImageROI(resizeBW,cvRect(camNum*640,0,640,480));
		cvSetImageROI(resize,cvRect(camNum*640,0,640,480));
		cvResize(resizeBW,arimg);
		getARtagPose(arimg, resize, tags, camNum);
		alltags[camNum] = tags;
		cvResetImageROI(resizeBW);
		cvResetImageROI(resize);
		if (!hidedisplay)
		{
			char str[30];
			cvResize(resize,dispImg);
			sprintf(str,"ts:%f ind:%d",timestamp,frameNum);
			cvRectangle(resize,cvPoint(10,2),cvPoint(170,10),cvScalar(0,0,0),-1);
			cvPutText (resize,str,cvPoint(10,10),&font,cvScalar(255,255,255));
			cvShowImage("output",dispImg);
			displayhidden = false;
		}
		else
		{
			if(!displayhidden)
			{
				cvDestroyWindow("output");
				displayhidden = true;
			}
			cvNamedWindow ("output",CV_WINDOW_AUTOSIZE);
			cvResizeWindow("output",0,0);
		}

		// construct protobuff stuff...
		ARTag::ARTagMessage* arTags = new ARTag::ARTagMessage();// ARTag (repeated = arrays) for Protobuff
		arTags->set_timestamp(timestamp);
		arTags->set_camera_id(camNum);
		for(int i = 0; i < tags.size(); i++)
		{
			cv::Mat pose = tags[i].getPose();
			ARTag::ARTagMessage_Pose* tagPose;
			tagPose = arTags->add_oois();
			tagPose->set_timestamp(timestamp);
			tagPose->set_x(pose.at<float>(0,3) / 1000.0);
			tagPose->set_y(pose.at<float>(1,3) / 1000.0);
			tagPose->set_z(pose.at<float>(2,3) / 1000.0);
			tagPose->set_id(tags[i].getId());
			tagPose->set_yaw(atan2(pose.at<float>(1,0), pose.at<float>(0,0)));

			// add field in ARTagMessage_Pose to put pitch and roll***********************************
			float cos_pitch = sqrt(pose.at<float>(0,0)*pose.at<float>(0,0) + pose.at<float>(0,1)*pose.at<float>(0,1));
			tagPose->set_pitch(atan2(-pose.at<float>(2,0), cos_pitch));
			tagPose->set_roll(atan2(pose.at<float>(2,1), pose.at<float>(2,2)));
		}

		
		if (usePipes) {
			pPipe->WriteMessage(arTags);
		} else {
			sender->Send(arTags);
		}
		
		delete arTags;
	}
	DisplayMsgInConsole();
	//Sleep(10);
}

void RunRealtime()
{	
	// Create accessore object for camera data.
	CamDataReader dataAccessor(camName.c_str(), CamDataWriter::DEFAULT_SEMAPHORE_NAME, CamDataWriter::DEFAULT_MUTEX_NAME);

	t1=clock();
    t2=clock();

	char filename[300];
	//DWORD lastTicks = GetTickCount();
	while(running)
	{
		#pragma region Handle user input
		char buffer[640*480*3] = {0};
		switch(cvWaitKey (1))
		{
			case 'q':
				running=false;			
				break;
			case 'c':
				sprintf(filename, "c:\\img_%05d.jpg", frameNum);
				cvSaveImage (filename,resize);
				printf("Current image saved to %s\n", filename);
				break;
			case 'd':
				cvSetImageROI(resize,cvRect(640,0,640,480));
				sprintf(filename, "c:\\img_%05d.jpg", calibframeNum++);
				cvSaveImage (filename,resize);
				printf("Current image saved to %s\n", filename);
				cvResetImageROI(resize);
				break;
			case 's':
				hidedisplay = !hidedisplay;
				break;
			case 'l':
				logcam = !logcam;
				break;
			default:
				break;
		}

		#pragma endregion

		// Get image data
		dataAccessor.ReadNextImage(img->imageData, WIDTH*HEIGHT*CHANNELS, &timestamp);

		// begin time of processing
		t1=clock();

		if(CHANNELS == 1)
			cvCvtColor(img,resize,CV_BayerBG2BGR);
		else
			cvResize(img,resize);

		RunCamera(timestamp);	
		t2=clock();
		printf("\nARtagLocalizer processing time: \t%f\n", ((float)t2-(float)t1) / CLOCKS_PER_SEC);
		printf("Difference in timestamp of the image: \t%f\n", actTimestamp - timestamp);
		

		if(oldtimestamp != timestamp)
		{
			frameNum++;	
			oldtimestamp = timestamp;
		}
	}
	/*CloseHandle(hMapFile);	*/
	cvReleaseImage(&colorImg);
}

void printUsage() {
	printf("ARTagLocalizer [-p [<pipe name>]] [-c [<camera name>]] [<calibration file> ...]\n"
		"\t-p\tThis flag specifies that you want to use named pipes for \n"
		"\t\tcommunication. The optional <pipe name> argument allows you\n"
		"\t\tto specify the name of the pipe.\n"
		"\t-c\tThis flag allows you to specify the name of the mutex \n"
		"\t\tobject used to protect access to the image memory location \n"
		"\t\tusing <camera name>."
		);
}

void processArgs(int argc, char **argv) {
	int numCams = 1;
	camParamFiles[0] = "PGR_FireflyMV.cal";
	int unflaggedArgs = 0;

	if (argc>1)
	{
		for (int i = 1; i < argc; ++i) {

			if (argv[i][0] == '-') {
				if (argv[i][1] == 'p') {
					usePipes = true;
					if (argc > i+1 && argv[i+1][0] != '-') {
						pipeName = argv[++i];
					}
				} else if (argv[i][1] == 'c') {
					if (argc > i+1 && argv[i+1][0] != '-') {
						camName = argv[++i];
					}
				}
				if (argv[i][1] == 'h' || argv[i][1] == '?') {
					printUsage();
					exit(0);
				}
			} else {
				++unflaggedArgs;
				switch (unflaggedArgs) {
				case 1:
					if (strcmp(argv[i],"FIREFLY")==0) 
					{
						numCams = 3;
						WIDTH = WIDTH*3;
						HEIGHT = 480;
						CHANNELS = 1;	
						printf ("Using PGR Firefly.\n");			
					}
					else if (strcmp(argv[i],"PLAYLOG")==0) 
					{
						CHANNELS = 1;		
						printf ("Playing logged data.\n");	
					}
					else
					{
						CHANNELS = 3;			
						printf ("Using Unibrain.\n");			
					}
					break;
				case 2:
					numCams = atoi(argv[i]);
					camParamFiles.resize(numCams);
					alltags.resize(numCams);
					for (int j = 0; j < numCams; ++j) {
						camParamFiles[j] = "PGR_FireflyMV.cal";
					}
					break;
				case 3:
					for (int j = 0; j < numCams; ++j) {
						if (argc > i) {
							camParamFiles[j] = argv[i++];
						} else {
							camParamFiles[j] = "PGR_FireflyMV.cal";
						}
					}
					break;
				default:
					break;
				}
			}
		}

	} else {
		printf("Warning: No Command Line Arguments Defined. Using Defaults.\n");
	}
}

int main(int argc, char **argv)
{
	camName = CamDataWriter::DEFAULT_CAM_NAME;
	processArgs(argc, argv);		

	SetProcessAffinityMask (GetCurrentProcess (),0x01);
	SetErrorMode(0x07);

	Sleep(2000);

	robotID = GetRobotID() % 10;
	cout<<"ARtagLocalizer"<<endl;
	cout<<"Robot ID: " << robotID << endl;
	cout<<endl;
	InitCommon();
	Sleep(100);
	RunRealtime();	
	return 0;
}

