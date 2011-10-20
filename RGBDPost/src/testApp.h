#pragma once

#include "ofMain.h"
#include "ofxRGBDAlignment.h"
#include "ofxCvCheckerboardPreview.h"
#include "ofxCVgui.h"
#include "scrGameView3D.h"


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
	void exit();
	
	ofxRGBDAlignment alignment;		
	void calibrateFromDirectory();
	void loadPreviewSequence();
	
	bool framesLoaded;
	int totalFrames;
	int currentFrame;
	
//	ofxCvCheckerboardPreview kinectCheckerPreview;
//	ofxCvCheckerboardPreview cameraCheckerPreview;
	
	vector< vector<ofVec3f> > kinectClouds;
	vector<ofImage> externalImages;
	
	ofxCVgui* gui;
	scrGroupTabbed* mainScreen;
	scrGroupGrid* calibrate;
	scrGroupGrid* preview;
	scrDraw2D* kinectView;
	scrDraw2D* depthView;
	scrGameView3D* pointcloudView;
	scrWidgets* tools;
		
	void drawOnKinect(ofRectangle& drawRect);
	void drawOnCamera(ofRectangle& drawRect);
	void drawOnPoint(ofNode& drawNode);
	

};
