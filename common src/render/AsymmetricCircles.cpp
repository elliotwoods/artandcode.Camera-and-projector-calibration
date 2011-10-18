//
//  AsymmetricCircles.cpp
//  ProjectChessboard-client
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#include "AsymmetricCircles.h"

AsymmetricCircles::AsymmetricCircles() :
scale(0.9f),
squaresX(7), squaresY(5),
whiteBackground(true),
brightness(0.5) {
	
}

void AsymmetricCircles::draw(float x, float y) {
	this->draw(x, y, ofGetViewportWidth(), ofGetViewportHeight());
}

void AsymmetricCircles::draw(float x, float y, float w, float h) {
	
	ofPushStyle();
	ofSetLineWidth(0.0f);
	ofFill();
	
	if (whiteBackground)
		ofSetColor(255.0f * brightness, 255.0f * brightness, 255.0f * brightness);
	else
		ofSetColor(0, 0, 0);
	ofRect(x, y, w, h);
		
	float xstep = w / squaresX * scale;
	float ystep = h / squaresY * scale;
	
	float xInset = x + (w - (w * scale)) / 2.0f;
	float yInset = y + (h - (h * scale)) / 2.0f;
	
	float color = 0.0f;
	
	for (int i=0; i<squaresX; ++i)
		for (int j=0; j<squaresY; ++j) {
			
			color = 255.0f * brightness * (i%2 == j%2);
			ofSetColor(color, color, color);
			
			ofRect(xInset + xstep*i, yInset + ystep*j, xstep, ystep);
			
		}
	
	ofPopStyle();
}

vector<ofVec2f> AsymmetricCircles::getProjectionSpaceCorners() const {
	
	vector<ofVec2f> corners;
	
	ofVec2f step = scale * ofVec2f(2.0f / float(squaresX),
									 2.0f / float(squaresY));
	
	ofVec2f inset = ofVec2f(1.0f - scale, 1.0f - scale);
	
	for (int i=1; i<squaresX; ++i)
		for (int j=1; j<squaresY; ++j) {
			
			corners.push_back(inset + step * ofVec2f(i, j));
			
		}
	
	return corners;
}

bool AsymmetricCircles::findFeatures(ofPixels &image, vector<ofVec2f> &points) const {
    bool found;
	
	Mat img = toCv(image);
    
	//found = findChessboardCorners(img, cv::Size(squaresX, squaresY), *(vector<ofVec2f>*)&points, CV_CALIB_CB_ADAPTIVE_THRESH);
	/*
    // improve corner accuracy
    if(found) {
		cornerSubPix(img, points, cv::Size(subPixSearch, subPixSearch),  cv::Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));
    }
	 */
    
    return found;
}