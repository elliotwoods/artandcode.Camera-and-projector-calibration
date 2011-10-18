//
//  CorrelateXYZtoXY.cpp
//  CalibrateProjector
//
//  Created by Elliot Woods on 14/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#include "CorrelateXYZtoXY.h"

CorrelateXYZtoXY::CorrelateXYZtoXY() {
	
	cvParameters.insert(pair<string, bool>("CV_CALIB_USE_INTRINSIC_GUESS", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_PRINCIPAL_POINT", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_ASPECT_RATIO", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_ZERO_TANGENT_DIST", true));
	
	projectorResolution.x = 1024;
	projectorResolution.y = 768;

	clear();
	
	xyzPreview.setMode(OF_PRIMITIVE_POINTS);
}

void CorrelateXYZtoXY::push(ofVec3f &xyz, ofVec2f &xy) {
	if (xyz.length() < 0.1)
		return;
	
	++count;
	this->xyz.push_back(xyz);
	this->xy.push_back(xy);
	
	xyzPreview.addVertex(xyz);
}

void CorrelateXYZtoXY::clear() {
	xyz.clear();
	xy.clear();
	xyzPreview.clear();
	count=0;
	error=0;
}

int CorrelateXYZtoXY::size() {
	return xyz.size();
}

void CorrelateXYZtoXY::draw() {
	ofPushStyle();
	xyzPreview.draw();
	ofPopStyle();
}

void CorrelateXYZtoXY::correlate() {
	if (size() < 11)
		return;
	
	vector<Mat> boardRotations, boardTranslations;
	
	vector<vector<Point3f> > vvo(1); //object points
	vector<vector<Point2f> > vvi(1); //image points
	
	
	int flags = 0;
	
	flags |= cvParameters["CV_CALIB_USE_INTRINSIC_GUESS"] & CV_CALIB_USE_INTRINSIC_GUESS;
	flags |= cvParameters["CV_CALIB_FIX_PRINCIPAL_POINT"] & CV_CALIB_FIX_PRINCIPAL_POINT;
	flags |= cvParameters["CV_CALIB_FIX_ASPECT_RATIO"] & CV_CALIB_FIX_ASPECT_RATIO;
	flags |= cvParameters["CV_CALIB_ZERO_TANGENT_DIST"] & CV_CALIB_ZERO_TANGENT_DIST;
	
	//pull values into the 3d calibration
	vvo[0] = *(vector<Point3f >*)&xyz;
	vvi[0] = *(vector<Point2f >*)&xy;
	
	//initialised throw ratio = 1.0f
	float f = projectorResolution.x;
	cameraMatrix = (Mat1d(3, 3) <<
					f, 0, f/2,
					0, f, f/2,
					0, 0, 1);

	error = calibrateCamera(vvo, vvi, cv::Size(projectorResolution.x, projectorResolution.y), cameraMatrix, distCoeffs, boardRotations, boardTranslations, flags);
	
	cout << cameraMatrix << endl;
	cout << distCoeffs << endl;
	
}

ofVec2f CorrelateXYZtoXY::project(const ofVec3f xyz) const {
	Mat1f mo(3, 1), mt(3, 1), mr(3, 1);
	
	*(ofVec3f*)&mo(0,0) = xyz;
	*(ofVec3f*)&mt(0,0) = ofVec3f(0,0,0);
	*(ofVec3f*)&mr(0,0) = ofVec3f(0,0,0);
	
	vector<Point2f> projected;
	
	projectPoints(mo, mr, mt, cameraMatrix, distCoeffs, projected);
	
	return toOf(projected[0]);
}