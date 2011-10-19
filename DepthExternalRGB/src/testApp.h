#pragma once

#include "ofMain.h"
#include "ofxDepthRGBAlignment.h"
#include "ofxKinect.h"
#include "ofxCvCheckerboardPreview.h"

#include "ofxCVgui.h"

using namespace ofxCv;
using namespace cv;

class testApp : public ofBaseApp{

  public:
	testApp();
	
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
	
	ofxCvCheckerboardPreview kinectCheckerPreview;
	ofxCvCheckerboardPreview cameraCheckerPreview;
	ofxDepthRGBAlignment depthRGBAlignment;		
	ofImage grayCopy;
	ofVideoGrabber rgbcamera;
	ofxKinect kinect;

	ofxCVgui gui;
	scrGroupGrid mainScreen;
	scrDraw2D kinectView;
	scrDraw2D cameraView;
	void drawOnKinect(ofRectangle& drawRect);
	void drawOnCamera(ofRectangle& drawRect);

	
};
