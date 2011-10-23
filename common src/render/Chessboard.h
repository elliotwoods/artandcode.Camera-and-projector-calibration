//
//  Chessboard.h
//  ProjectChessboard-client
//
//  Created by Elliot Woods on 13/10/2011.
//  Copyright 2011 Kimchi and Chips.
//
//	Distrubuted under the MIT license
//	http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "ofMain.h"
#include "ofxCv.h"

#define MAX_MARKERS 512

using namespace cv;
using namespace ofxCv;

class BoardMarker {
public:
	BoardMarker();
	bool enabled;
	ofVec2f xy;
};

class Chessboard : public ofBaseDraws {
public:
	Chessboard();
	
	///////////////
	// ofBaseDraws
	///////////////
	//
	virtual void draw(float x,float y);
	virtual void draw(float x,float y,float w, float h);
	/**
	 Since the drawing is scale invariant, so these functions don't have their usual meaning.
	 */
	virtual float getHeight() { return squaresX; };
	virtual float getWidth() { return squaresY; };
	//
	///////////////
	
	void setScale(float s) { scale = s; };
	void setWhiteBackground(bool b) {whiteBackground = b; }

	///Returns the projection space coordinates of the inner corners
	vector<ofVec2f>	getProjectionSpaceCorners() const;
	
	///Find chessboard corners within a camera image
	bool	findCorners(ofPixels &image, vector<ofVec2f> &points) const;
	
	
	float	squaresX, squaresY; //number of squares
	float	scale; ///<Relative to viewport. 1=fill viewport, 0=nothing
	bool	whiteBackground;
	float	brightness;
	
	BoardMarker markers[MAX_MARKERS];
};