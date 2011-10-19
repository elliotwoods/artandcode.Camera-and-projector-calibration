/*
 *  ofxDepthRGBAlignment.h
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/18/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;

class ofxDepthRGBAlignment {
  public:
	ofxDepthRGBAlignment();
	~ofxDepthRGBAlignment();
	
	void setup(int squaresWide, int squaresTall, int squareSize);
	
	bool addCalibrationImagePair(ofPixels& ir, ofPixels &camera);	//TODO: set calibration files
	bool setCalibrationDirectoryPair(string irImageDirectory, string colorImageDirectory);
	bool ready();
//	void saveCalibration(string filename);

	//Display of dat
	void setColorImage(ofPixels& colorImage);
	void setDepthImage(unsigned short* pixels);
	void update(); //updates current mesh

	void drawMesh();
	void drawPointCloud();
	
	void saveCalibration();
	void loadCalibration();
	void resetCalibration();
	void setPoinCloud(vector<Point3f>& newCloud);
	
	ofVec3f getMeshCenter();
	float getMeshDistance();
	
  protected:
	bool hasDepthImage;
	bool hasColorImage;
	bool hasPointCloud;
	
	ofPixels currentColorImage;
	unsigned short* currentDepthImage;
	
	void updatePointCloud();
	void updateColors();
	
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