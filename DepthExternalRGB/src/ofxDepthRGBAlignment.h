/*
 *  ofxDepthRGBAlignment.h
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/18/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

class ofxDepthRGBAlignment {
  public:
	ofxDepthRGBAlignment();
	~ofxDepthRGBAlignment();
	void setup();
	
	//calibratin of matrix
	//TODO: set calibration files or directories
	void addCalibrationPair(ofImage depthImage, ofImage colorImage);
	void computeCalibration();
	
	//Display of dat
	void setColorImage(ofImage colorimage);
	void setDepthImage(ofImage depthImage);
	void update(); //updates current mesh
	

	void drawMesh();
	void drawPointCloud();
	
  protected:
	ofVboMesh mesh;
	Calibration kinectCalibration, colorCalibration;    
	Mat rotationKinectToColor, translationKinectToColor;
	Mat rotationColorToKinect, translationColorToKinect;
	
	Mat rotation, translation;
	
};