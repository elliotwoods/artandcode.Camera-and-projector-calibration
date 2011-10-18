/*
 *  ofxCvCheckerboardPreview.cpp
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/18/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#include "ofxCvCheckerboardPreview.h"


void ofxCvCheckerboardPreview::setup(){
	boardFound = false;
	newBoardToFind = false;
	
	calib.setPatternSize(10,7);
	calib.setSquareSize(4);

	startThread(true, false);

}

void ofxCvCheckerboardPreview::setTestImage(ofPixels& pixels) {
	
	lock();
	testImage = pixels;
	if(!internalPixels.isAllocated()){
		internalPixels.allocate(pixels.getWidth(), pixels.getHeight(), OF_PIXELS_MONO);
	}
	newBoardToFind = true;
	boardFound = false;
	unlock();
}

bool ofxCvCheckerboardPreview::currentImageHasCheckerboard() {
	return boardFound;
}

void ofxCvCheckerboardPreview::draw(ofVec2f point){
//void ofxCvCheckerboardPreview::drawCurrentBoard(ofVec2f point){
	
	vector<Point2f> points;
	drawLock.lock();
	points = imagePoints;
	drawLock.unlock();
	
	float x, y;
	for (int i=0; i < imagePoints.size(); ++i) {
//				x = imagePoints[i].x / width * testImage.getHeight() + point.x;
//				y = imagePoints[i].y / height * testImage.getWidth() + point.y;
		x = imagePoints[i].x + point.x;
		y = imagePoints[i].y + point.y;
		
		ofCircle(x, y, 5);
	}
	
}

void ofxCvCheckerboardPreview::threadedFunction() {
	
	while (isThreadRunning()){
		if(newBoardToFind){
			lock();
			memcpy(internalPixels.getPixels(), testImage.getPixels(), testImage.getWidth()*testImage.getHeight()); 
			unlock();
			
			vector<Point2f> points;
			boardFound = calib.findBoard(toCv(internalPixels), points);
			if (boardFound){
				drawLock.lock();
				imagePoints = points;
				drawLock.unlock();
			}
			
			newBoardToFind = false;
		}
	}
}

void ofxCvCheckerboardPreview::quit() {
	
}