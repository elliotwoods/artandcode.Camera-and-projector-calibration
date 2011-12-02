#pragma once

#include "ofMain.h"

#include "ofxCV.h"

using namespace cv;
using namespace ofxCv;
class testApp : public ofBaseApp{

public:
	testApp();
	
	void setup();
	void update();
	void draw();
	
	ofVec2f project(ofVec3f xyz);

	vector<ofVec3f> xyz;
	vector<ofVec2f> xy;

	Mat cameraMatrix;
	Mat distCoeffs;
	Intrinsics intrinsics;
	
	float error;
};
