//
//  Chessboard.cpp
//  ProjectChessboard-client
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#include "Chessboard.h"

BoardMarker::BoardMarker() {
	enabled = false;
}

Chessboard::Chessboard() :
scale(0.7f),
squaresX(7), squaresY(5),
whiteBackground(true),
brightness(0.3) {
	
}

void Chessboard::draw(float x, float y) {
	this->draw(x, y, ofGetViewportWidth(), ofGetViewportHeight());
}

void Chessboard::draw(float x, float y, float w, float h) {
	
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
	
	ofPushStyle();
	ofEnableSmoothing();
	ofSetLineWidth(3.0f);
	ofSetColor(255,0,0);
	ofNoFill();
	for (int i=0; i<MAX_MARKERS; ++i) {
		if (!markers[i].enabled || markers[i].xy.x < 0 || markers[i].xy.y < 0 || markers[i].xy.x > 1 || markers[i].xy.y > 1)
			continue;
		ofCircle(markers[i].xy.x * w + x , markers[i].xy.y * h + y, 10);
	}
	ofPopStyle();
}

vector<ofVec2f> Chessboard::getProjectionSpaceCorners() const {
	
	vector<ofVec2f> out;
	
	ofVec2f step = ofVec2f(2.0f / float(squaresX),
						   2.0f / float(squaresY));
	
	ofVec2f inset = ofVec2f(-1.0f, 1.0f);
	
	ofVec2f corner, xyPix;
	for (int j=1; j<squaresY; ++j)
		for (int i=1; i<squaresX; ++i) {
			corner = inset + step * ofVec2f(i, -j);
			corner *= scale;
			xyPix.x = (corner.x + 1.0f) / 2.0f * 1280;
			xyPix.y = (1.0f - corner.y) / 2.0f * 800; 
			
			out.push_back(xyPix);
		}

	return out;
}

bool Chessboard::findCorners(ofPixels &image, vector<ofVec2f> &points) const {
	int subPixSearch = image.getWidth() / squaresX / 10.0f;
	if (subPixSearch % 2 == subPixSearch)
		subPixSearch++; //ensure odd numbers
	
    int chessFlags = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK;
    bool found;
	
	Mat img = toCv(image);
    
    found = findChessboardCorners(img, cv::Size(squaresX-1, squaresY-1), *(vector<Point2f>*)&points, chessFlags);
    
    return found;
}