//
//  scrScrub.cpp
//  RGBDPost
//
//  Created by James George on 10/22/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//


#include "scrScrub.h"

scrScrub::scrScrub(string caption)
: scrBase(caption)
{
    //
}

void scrScrub::mousePressed(float x, float y, int button){
    scrBase::mousePressed(x,y,button);
}

void scrScrub::mouseDragged(float x, float y, float dx, float dy, int button){
    
}

void scrScrub::keyPressed(int key) {

}

void scrScrub::drawContent() {
	ofPushStyle();
	ofSetColor(0);
    
	ofRectangle viewport = getLiveBounds();
	ofRect(	viewport );
	ofPopStyle();
	
	camera.begin(viewport);
	glEnable(GL_DEPTH_TEST);    
    
}