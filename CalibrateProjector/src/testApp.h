#pragma once

#include "ofMain.h"

#include "ofxCVgui.h"
#include "ofxOpenNI.h"
#include "ofxTalky.h"

#include "Chessboard.h"
#include "OpenNI2ViewNode.h"

#include "CorrelateXYZtoXY.h"

class testApp : public ofBaseApp{

public:
	testApp();
	
	void setup();
	void update();
	void draw();

protected:
	
	//////////////////
	// Kinect
	//////////////////
	//
	ofxOpenNI		kinect;
	
	void			pipetRGB(ofVec2f &m);
	ofVec3f			worldCursor;
	//
	//////////////////
	
	
	//////////////////
	// Network
	//////////////////
	//
	ofxTalky		client;
	//
	//////////////////
	
	OpenNI2ViewNode	kinectView;
	
	//////////////////
	// GUI
	//////////////////
	//
	ofxCVgui		screens;
	scrGroupGrid	scrMain;
	
	scrDraw2D		scrPreviewBoard;
	scrDraw2D		scrPreviewRGB;
	scrDraw3D		scrKinectView;
	scrWidgets		scrControl;
	
	wdgSlider		wdgScale;
	wdgButton		wdgWhiteBackground;
	wdgSlider		wdgBrightness;
	wdgButton		wdgCapture;
	wdgSlider		wdgCursor;
	wdgSlider		wdgError;
	//
	//////////////////
	
	
	//////////////////
	// Chessboard
	//////////////////
	//
	void	drawFoundCorners2D(ofRectangle &r);
	void	drawFoundCorners3D(ofNode &n);
	
	Chessboard			board;
	vector<ofVec2f>		projectedCornersP;
	vector<ofVec2f>		foundCornersC;
	vector<ofVec3f>		foundCornersW;
	//
	//////////////////
	
	
	//////////////////
	// Correlation
	//////////////////
	//
	void				capture();
	
	CorrelateXYZtoXY	correlation;
	bool				showMarkers;
	//
	//////////////////
};
