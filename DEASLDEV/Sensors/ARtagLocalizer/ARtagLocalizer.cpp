#include "ARtagLocalizer.h"
#include "ARToolKitPlus/TrackerSingleMarkerImpl.h"
#include "Proto/ARTagOutput.pb.h"

using namespace std;

const float THIN_PATTERN_BORDER = 0.125;
const float THICK_PATTERN_BORDER = 0.25;

int numCams_ = 1;
int imgwidth = 640;
int imgheight = 480;
const bool    useBCH = true;
bool init = false;

float patternWidth_;
float patternCenter_[2];
unsigned char *cameraBuffer;
ARToolKitPlus::TrackerSingleMarker **trackers;

int initARtagPose(int width, int height, float markerWidth, vector<string> camParamFiles)
{
	size_t numPixels = width*height;
	cameraBuffer = new unsigned char[numPixels];
	imgwidth = width;
	imgheight = height;
	patternCenter_[0] = patternCenter_[1] = 0.0;
	patternWidth_ = markerWidth;
	numCams_ = camParamFiles.size();
	trackers = new ARToolKitPlus::TrackerSingleMarker *[numCams_];
	// create a tracker that does:
	//  - 6x6 sized marker images
	//  - samples at a maximum of 6x6
	//  - works with luminance (gray) images
	//  - can load a maximum of 1 pattern
	//  - can detect a maximum of 8 patterns in one image
	for (int i = 0; i < numCams_; ++i) {
		if (camParamFiles[i].compare("null") == 0) {
			trackers[i] = NULL;
			continue;
		}
		ARToolKitPlus::TrackerSingleMarker *tracker = new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, 8>(width,height);
		
		tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
		// load a camera file. 
		if(!tracker->init(camParamFiles[i].c_str(), 1.0f, 1000.0f))
		{
			printf("ERROR: init() failed\n");
			delete cameraBuffer;
			delete tracker;
			return -1;
		}

		// define size of the marker
		tracker->setPatternWidth(patternWidth_);

		// the marker in the BCH test image has a thin border...
		tracker->setBorderWidth(THIN_PATTERN_BORDER);

		// set a threshold. alternatively we could also activate automatic thresholding
		tracker->setThreshold(150);
		tracker->activateAutoThreshold(true);

		// let's use lookup-table undistortion for high-speed
		// note: LUT only works with images up to 1024x1024
		tracker->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

		// RPP is more robust than ARToolKit's standard pose estimator
		tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
		//tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL);

		// switch to simple ID based markers
		// use the tool in tools/IdPatGen to generate markers
		tracker->setMarkerMode(useBCH ? ARToolKitPlus::MARKER_ID_BCH : ARToolKitPlus::MARKER_ID_SIMPLE);

		trackers[i] = tracker;
	}

	init = true;
	return 0;
}

bool getARtagPose(IplImage * src, IplImage *dst, vector<ARtag>& tags, int camNum)
{
	if (trackers[camNum] == NULL) {
		// This camera does not have a tracker object.
		return false;
	}
	if (!init)
	{
		printf("Did not initalize the ARtagLocalizer!!\n");
		return NULL;
	}
	if (src->width != imgwidth || src->height != imgheight)
	{
		printf("Image passed in does not match initialized image size!!\n");
		return NULL;
	}
	if (src->nChannels != 1)
	{
		printf("Please pass in grayscale image into ARtagLocalizer! \n");
		return NULL;
	}
	
	int n = 0;
	for(int i = 0; i < src->height; ++i)
		for(int j = 0; j < src->width; ++j)
			cameraBuffer[n++] = CV_IMAGE_ELEM(src,uchar,i,j);

	/*const char* description = tracker->getDescription();
	printf("ARToolKitPlus compile-time information:\n%s\n\n", description);*/

	int numMarkers = 0;
	ARToolKitPlus::ARMarkerInfo* markers = NULL;
	if (trackers[camNum]->arDetectMarker(const_cast<unsigned char*>(cameraBuffer), 150, &markers, &numMarkers) < 0) 
	{
		return false;
	}

	float modelViewMatrix_[16];

	for(int m = 0; m < numMarkers; ++m) {
		if(markers[m].id != -1 && markers[m].cf >= 0.5) {
			trackers[camNum]->calcOpenGLMatrixFromMarker(&markers[m], patternCenter_, patternWidth_, modelViewMatrix_);

			ARtag ar;
			ar.setId(markers[m].id);

			float x = modelViewMatrix_[12] / 1000.0;
			float y = modelViewMatrix_[13] / 1000.0;
			float z = modelViewMatrix_[14] / 1000.0;
			float yaw = -atan2(modelViewMatrix_[1], modelViewMatrix_[0]);

			if ((x == 0 && y == 0 && yaw == 0) || (x > 10000 && y > 10000) || (x < -10000 && y < -10000) || z <= 0.01)
			{
				// ARTKPlus bug that occurs sometimes
				continue;
			}			
			
			char str[30];
			sprintf(str,"%d",markers[m].id);
			cvPutText (dst,str,cvPoint( markers[m].pos[0]+25,markers[m].pos[1]+10),&cvFont(3,3),cvScalar(0,0,255));
			sprintf(str,"(%.2f,%.2f,%.2f)", x, y, yaw);
			cvPutText (dst,str,cvPoint( markers[m].pos[0]+25,markers[m].pos[1]+25),&cvFont(1,1),cvScalar(0,0,255));

			cv::Mat PoseM(4, 4, CV_32F, modelViewMatrix_);
			cv::transpose(PoseM,PoseM);
			CvMat pose = PoseM;
			ar.setPose(&pose);
			ar.setPoseAge(0);
			tags.push_back(ar);
		}
	}


	//google::protobuf::ShutdownProtobufLibrary();
	//printf("\n\n");

	return true;
}

int cleanupARtagPose(void)
{
	delete [] cameraBuffer;
	for (int i = 0; i < numCams_; ++i) {
		delete trackers[i];
	}
	delete [] trackers;
	return 0;
}