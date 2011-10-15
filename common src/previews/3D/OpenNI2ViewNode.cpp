//
//  OpenNI2ViewNode.cpp
//  ofxOpenNI2-test
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#include "OpenNI2ViewNode.h"

OpenNI2ViewNode::OpenNI2ViewNode(ofxOpenNI & k) : 
kinect(k) {
	glGenBuffersARB(1, &vbo[0]);
	glGenBuffersARB(1, &vbo[1]);
	
	width = 640;
	height = 480;
	
	nPoints = width * height;
	Pxyz.allocate(width, height, OF_PIXELS_RGB);
	Wxyz.allocate(width, height, OF_PIXELS_RGB);
	
	ofVec3f *P = (ofVec3f*) Pxyz.getPixels();
	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i) {
			P->x = i;
			P->y = j;
			P->z = 0;
			++P;
		}
			
}

//-----

void OpenNI2ViewNode::update() {
	XnDepthPixel* d = kinect.getDepthRawPixels().getPixels();
	
	float *out = ((float*)Pxyz.getPixels()) + 2;
	for (int i=0; i<nPoints; ++i) {
		*out = float(*d) / 1000.0f;
		out +=3;
		++d;
	}
	kinect.getDepthGenerator().ConvertProjectiveToRealWorld(nPoints, (XnPoint3D*)Pxyz.getPixels(), (XnPoint3D*)Wxyz.getPixels());
}

//-----

void OpenNI2ViewNode::customDraw() {
	
	update();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, nPoints * 3 * sizeof(float), Wxyz.getPixels(), GL_STATIC_DRAW_ARB);
	glVertexPointer(3, GL_FLOAT, 0, 0);
    
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, nPoints * 3 * sizeof(unsigned char), kinect.getRGBPixels().getPixels(), GL_STATIC_DRAW_ARB);
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);
	
	
	glPointSize(2.0f);
	glDrawArrays(GL_POINTS, 0, nPoints);
	
	
	glDisableClientState(GL_VERTEX_ARRAY); 
	glDisableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}