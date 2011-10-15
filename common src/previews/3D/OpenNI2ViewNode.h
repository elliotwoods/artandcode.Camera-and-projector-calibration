//
//  OpenNI2ViewNode.h
//  ofxOpenNI2-test
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"

/** 3D node view of Kinect data.
 We presume that kinect.update() is already being called.
 or we're using OpenNI's threading
 */
class OpenNI2ViewNode : public ofNode {
public:
	OpenNI2ViewNode(ofxOpenNI& k);
	
	ofVec3f		lookup(ofVec2f xy);
	vector<ofVec3f>	markers;
	
protected:
	void		update();
	void		customDraw();
	
	ofxOpenNI&	kinect;
	
	ofFloatPixels	Pxyz;
	ofFloatPixels	Wxyz;
	
	GLuint			vbo[2];
	GLuint			nPoints;
	
	float			width, height;
};