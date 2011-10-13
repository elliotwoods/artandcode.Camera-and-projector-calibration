//
//  KinectViewNode.h
//  ofxKinect-test
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "ofMain.h"
#include "ofxKinect.h"

/** 3D node view of Kinect data.
 We presume that kinect.update() is already being called.
 */
class KinectViewNode : public ofNode {
public:
	KinectViewNode(ofxKinect& k);
	
protected:
	void		update();
	void		customDraw();
	
	ofxKinect&	kinect;
	
	ofFloatPixels	xyz;
	ofPixels		rgb;
	
	GLuint			vbo[2];
	GLuint			nPoints;
};