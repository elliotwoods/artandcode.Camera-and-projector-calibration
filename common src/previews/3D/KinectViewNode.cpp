//
//  KinectViewNode.cpp
//  ofxKinect-test
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#include "KinectViewNode.h"

KinectViewNode::KinectViewNode(ofxKinect& k) : 
kinect(k) {
	glGenBuffersARB(1, &vbo[0]);
	glGenBuffersARB(1, &vbo[1]);
	
	nPoints = kinect.width * kinect.height;
	xyz.allocate(kinect.width, kinect.height, OF_PIXELS_RGB);
	rgb.allocate(kinect.width, kinect.height, OF_PIXELS_RGB);
}

//-----

void KinectViewNode::update() {
	ofVec3f* currentXYZ = (ofVec3f*) xyz.getPixels();
	ofColor* currentRGB = (ofColor*) rgb.getPixels();
	
	for (int j=0; j<kinect.height; ++j)
		for (int i=0; i<kinect.width; ++i) {			
			*currentXYZ = kinect.getWorldCoordinateFor(i, j);
			*currentRGB = kinect.getCalibratedColorAt(i, j);
			
			++currentXYZ;
			++currentRGB;
		}
}

//-----

void KinectViewNode::customDraw() {
	
	update();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, nPoints * 3 * sizeof(float), xyz.getPixels(), GL_STATIC_DRAW_ARB);
	glVertexPointer(3, GL_FLOAT, 0, 0);
    
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, nPoints * 4 * sizeof(unsigned char), rgb.getPixels(), GL_STATIC_DRAW_ARB);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	
	
	ofPushMatrix();
	ofScale(-4.0f, -4.0f, -4.0f);
	glDrawArrays(GL_POINTS, 0, nPoints);
	ofPopMatrix();
	
	
	glDisableClientState(GL_VERTEX_ARRAY); 
	glDisableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}