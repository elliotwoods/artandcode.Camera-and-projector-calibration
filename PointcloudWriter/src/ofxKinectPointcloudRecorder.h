/*
 *  ofxKinectPointcloudRecorder.h
 *  PointcloudWriter
 *
 *  Created by Jim on 10/20/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxKinect.h";
#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;

class ofxKinectPointcloudRecorder : ofThread {
  public:
	ofxKinectPointcloudRecorder();
	void setup();
	void setRecordLocation(string directory, string filePrefix);
	
	void addImage(unsigned short* image);
	
	vector<ofVec3f> readPointcloud(string filename);
	vector<ofVec3f> readPointcloud(ofFile file);
	
	ofxKinect* kinect;

  protected:
	void threadedFunction();
	
	string targetDirectory;
	string targetFilePrefix;
	int currentFrame;
	queue<unsigned short*> saveQueue;
	bool isRecording;
};
