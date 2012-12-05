#include "ARtag.h"
#include "Network/ProtoBuffMulticast.h"

int initARtagPose(int width, int height, float markerWidth, vector<string> camParamFiles);
bool getARtagPose(IplImage * src, IplImage * dst, std::vector<ARtag> & tags, int camNum);
int cleanupARtagPose(void);
