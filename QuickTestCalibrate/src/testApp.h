#pragma once

#include "ofMain.h"
#include "ofxRGBDAlignment.h"
#include "ofxKinectPointcloudRecorder.h"
#include "ofxGameCamera.h"
#include "ofxCVgui.h"
#include "ofxXmlSettings.h"
#include "ofxCvCheckerboardPreview.h"

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
    ofxCVgui* gui;
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
    
    bool hideCalibrationDebug;
    bool videoFilesSet;
    
    string xmlSaveFile;
    ofxXmlSettings videosave;
    
    bool playing;
    bool showMesh;
    ofFbo fbo;
    ofPixels savePixels;
};
