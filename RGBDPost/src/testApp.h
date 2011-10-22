#pragma once

#include "ofMain.h"
#include "ofxRGBDAlignment.h"
#include "ofxKinectPointcloudRecorder.h"
#include "ofxCvCheckerboardPreview.h"
#include "ofxGameCamera.h"
#include "ofxCVgui.h"
#include "ofxXmlSettings.h"

using namespace ofxCv;
using namespace cv;

class testApp : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofImage testImage;
    ofxRGBDAlignment alignment;
    ofxKinectPointcloudRecorder decoder;
    
    int currentImage;
    vector<ofImage> externalImages;
    vector<ofImage> kinectImages;
    ofxCvCheckerboardPreview externalCheckers;
    ofxCvCheckerboardPreview kinectCheckers;
    
    ofVideoPlayer video;
    
    //GUI stuff
	//do you knw what would make this better?
    ofxCVgui* gui; // <---- THIS
	scrGroupTabbed* mainScreen;

	scrGroupGrid* loadSource;
	scrWidgets* clipWidgets;
	scrDraw2D* pointcloudTimeline;
	scrDraw2D* externalTimeline;
	
	scrGroupGrid* calibrate;
	scrWidgets* calibrateWidgets;
	scrDraw2D* externalCheckerView;
	scrDraw2D* kinectCheckerView;
	scrDraw3D* pointcloudView;
	
    ofTexture currentKinectCheckers;
    ofTexture currentExternalCheckers;
    
    ofCamera meshViewer;
    unsigned short* currentCloud;
    ofImage image;
    ofDirectory pointClouds;
    bool loaded;
    bool calibrationLoaded;
    bool calibrated;
    bool videoLoaded;
    ofxGameCamera cam;
    
    bool scrubVideo;
    int currentDepthFrame;
    int currentVideoFrame;
    
    bool frameASet;
    int depthFrameA;
    int videoFrameA;

    bool frameBSet;
    int depthFrameB;
    int videoFrameB;
    
    
    bool videoFilesSet;
    
    string xmlSaveFile;
    ofxXmlSettings videosave;

};
