/*
 *  ofxRGBDAlignment.h
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/18/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxKinect.h"

using namespace ofxCv;
using namespace cv;

class ofxRGBDAlignment {
  public:
	ofxRGBDAlignment();
	~ofxRGBDAlignment();
	
	void setup(int squaresWide, int squaresTall, int squareSize);
	
	bool addCalibrationImagePair(ofPixels& ir, ofPixels &camera);	//TODO: set calibration files
	bool calibrateFromDirectoryPair(string irImageDirectory, string colorImageDirectory);
	bool ready();
//	void saveCalibration(string filename);

	void setColorImage(ofImage& colorImage);
	void setDepthImage(unsigned short* pixels);
	void updatePointCloud(ofxKinect& kinect);
    void updatePointCloud(unsigned short* depthPixelsRaw, int w, int h);
    
	void update(); //updates current mesh

	void drawMesh();
	void drawPointCloud();
	
	void saveCalibration();
	void loadCalibration();
	void resetCalibration();

//	void updatePointCloud(vector<Point3f>& newCloud);
	void drawCalibration(bool left);
    
	ofVec3f getMeshCenter();
	float getMeshDistance();
	
  protected:
	bool hasDepthImage;
	bool hasColorImage;
	bool hasPointCloud;
	
//	vector<ofIndexType> indeces;
//	vector<ofVec2f> texcoords;
//	vector<ofVec3f> vertices;
	
	ofImage currentColorImage;
	unsigned short* currentDepthImage;
	
	void updatePointCloud();
	void updateColors();
	void updateMesh();
	
	//TEMP until we convert to mesh
	vector<Point2f> imagePoints;
	vector<Point3f> pointCloud;
	vector<Point3f> pointCloudColors;
	
	ofVboMesh mesh;
	
	Calibration depthCalibration, colorCalibration;    
	Mat rotationDepthToColor, translationDepthToColor;
	Mat rotationColorToDepth, translationColorToDepth;
	
	Mat rotation, translation;
	
	ofVec3f meshCenter;
	float meshDistance;
	
};