//
//  CorrelateXYZtoXY.h
//  CalibrateProjector
//
//  Created by Elliot Woods on 14/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "ofMain.h"

#include "ofxCv.h"

using namespace cv;
using namespace ofxCv;

class CorrelateXYZtoXY {
public:
	CorrelateXYZtoXY();
	
	void	push(ofVec3f &xyz, ofVec2f &xy);
	void	clear();
	int		size();
	
	void	draw();
	
	void	correlate();
	ofVec2f	project(const ofVec3f xyz) const;
	
	ofVec2f projectorResolution;
	
	vector<ofVec3f>	xyz;
	vector<ofVec2f>	xy;
	
	unsigned int	count;
	float			error;
	Mat				cameraMatrix, distCoeffs;
	
protected:
	ofMesh	xyzPreview;
	
	map<string, bool> cvParameters;
};