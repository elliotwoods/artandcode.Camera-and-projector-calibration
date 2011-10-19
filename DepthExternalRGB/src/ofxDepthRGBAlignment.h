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
	
	void setup();
	
	void addCalibrationImagePair(ofPixels& depth, ofPixels &color);	//TODO: set calibration files
	void setCalibrationDirectoryPair(string depthImageDirectory, string colorImageDirectory);
	
//	void saveCalibration(string filename);

	//Display of dat
	void setColorImage(ofPixels& colorImage);
	void setDepthImage(unsigned short* pixels);
	void update(); //updates current mesh

	void drawMesh();
	void drawPointCloud();
		
  protected:
	bool hasDepthImage;
	bool hasColorImage;
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
	
};