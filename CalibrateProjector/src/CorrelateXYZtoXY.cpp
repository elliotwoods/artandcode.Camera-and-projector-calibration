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
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_PRINCIPAL_POINT", false));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_ASPECT_RATIO", false));
	cvParameters.insert(pair<string, bool>("CV_CALIB_ZERO_TANGENT_DIST", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_K1", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_K2", true));
	cvParameters.insert(pair<string, bool>("CV_CALIB_FIX_K3", true));
	
	
	projectorResolution.x = 1280;
	projectorResolution.y = 800;

	clear();
	
	xyzPreview.setMode(OF_PRIMITIVE_POINTS);
}

void CorrelateXYZtoXY::push(ofVec3f &xyz, ofVec2f &xy) {
	if (xyz.length() < 0.1)
		return;
	
	++count;
	this->xyz.push_back(toCv(xyz));
	this->xy.push_back(toCv(xy));
	
	xyzPreview.addVertex(xyz);
}

void CorrelateXYZtoXY::clear() {
	xyz.clear();
	xy.clear();
	xyzPreview.clear();
	count=0;
	error=0;
	ready=false;
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
	flags |= cvParameters["CV_CALIB_FIX_K1"] & CV_CALIB_FIX_K1;
	flags |= cvParameters["CV_CALIB_FIX_K2"] & CV_CALIB_FIX_K2;
	flags |= cvParameters["CV_CALIB_FIX_K3"] & CV_CALIB_FIX_K3;
	
	
	//pull values into the 3d calibration
	//vvo[0] = *(vector<Point3f >*)&xyz;
	
	for (int i=0; i<xy.size(); ++i) {
		vvo[0] = xyz;
		vvi[0] = xy;
	}
	
	//initialised throw ratio = 1.0f
	float f = projectorResolution.y;
	cameraMatrix = (Mat1d(3, 3) <<
					f, 0, projectorResolution.x / 2.,
					0, f, projectorResolution.y / 2.,
					0, 0, 1);
	try {
		error = calibrateCamera(vvo, vvi, cv::Size(projectorResolution.x, projectorResolution.y), cameraMatrix, distCoeffs, boardRotations, boardTranslations, flags);
		ready=true;
	} catch (...) {
		ofLogError() << "Calibration threw exception. Check parameters.";
	}
	
	for(int i = 0; i < boardRotations.size(); i++) {
		cout << Mat(boardRotations[i]) << " " << boardTranslations[i] << endl;
	}
	
	Mat mt = (Mat1d(3,1) << 0, 0, 0);
	Mat mr = (Mat1d(3,1) << 0, 0, 0);
	vector<Point2f> projected;
	projectPoints(Mat(xyz), mr, mt, cameraMatrix, distCoeffs, projected);
	for(int i = 0; i < projected.size(); i++) {
		cout << Mat(xyz[i]) << "->" << Mat(projected[i]) << endl;
	}
	
	
	intrinsics.setup(cameraMatrix, cv::Size(projectorResolution.x, projectorResolution.y));
	
	cout << cameraMatrix << endl;
	cout << distCoeffs << endl;
	/*
	ofVec2f calced;
	for (int i=0; i<xyz.size(); i++) {
		cout << ofToString(xy[i].x,0) << "," << ofToString(xy[i].y,0);
		calced = project(xyz[i]);
		cout << " -> ";
		cout << ofToString(calced.x,0) << "," << ofToString(calced.y,0);
		cout << endl;
	}*/
}

ofVec2f CorrelateXYZtoXY::project(const Point3f o) const {
	vector<cv::Point3f> vo(1, o);
	
	Mat mt = (Mat1d(3,1) << 0, 0, 0);
	Mat mr = (Mat1d(3,1) << 0, 0, 0);
	
	vector<Point2f> projected;
	
	projectPoints(Mat(vo), mr, mt, cameraMatrix, distCoeffs, projected);
	
	return toOf(projected[0]);
}