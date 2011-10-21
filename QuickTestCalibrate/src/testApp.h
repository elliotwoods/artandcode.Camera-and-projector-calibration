#pragma once

#include "ofMain.h"
#include "ofxRGBDAlignment.h"
#include "ofxKinectPointcloudRecorder.h"
#include "ofxGameCamera.h"

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
    
    ofVideoPlayer video;
    
    unsigned short* currentCloud;
    ofImage image;
    ofDirectory pointClouds;
    bool loaded;
    bool calibrated;
    bool videoLoaded;
    ofxGameCamera cam;
};
