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

void CorrelateXYZtoXY::push(ofVec3f &xyz, ofVec2f &xy) {
	this->xyz.push_back(xyz);
	this->xy.push_back(xy);
	
	xyzPreview.addVertex(xyz);
}

void CorrelateXYZtoXY::clear() {
	xyz.clear();
	xy.clear();
	xyzPreview.clear();
}

int CorrelateXYZtoXY::size() {
	return xyz.size();
}

void CorrelateXYZtoXY::drawXYZ() {
	ofPushStyle();
	xyzPreview.draw();
	ofPopStyle();
}