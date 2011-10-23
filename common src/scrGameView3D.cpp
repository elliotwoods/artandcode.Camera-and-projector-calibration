/*
 *  scrGameView3D.cpp
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/19/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#include "scrGameView3D.h"

scrGameView3D::scrGameView3D(string caption, string cameraPositionFile) 
:
//	_node(node),
	scrBase(caption)
{
	//ofAddListener(ofEvents.keyPressed, this, &scrDraw3D::keyPressed);
//	camera.setDistance(1.0f);
///	camera.setNearClip(0.01f);
//	camera.speed = 15;
//	camera.autosavePosition = true;
//	camera.useArrowKeys = false;
//	camera.cameraPositionFile = cameraPositionFile;
//	camera.loadCameraPosition();
}

void scrGameView3D::drawContent()
{
	ofPushStyle();
	ofSetColor(255);

	ofRectangle viewport = getLiveBounds();
	ofRect(	viewport );
	ofPopStyle();
	
    ofVec3f spinVec(1,0,0);
    spinVec.rotate(ofGetMouseX(), ofVec3f(0,1,0));
    camera.setPosition( spinVec * 600 + ofVec3f(0,250,1500) );
    camera.lookAt(ofVec3f(0, 250, 1500));

	camera.begin(viewport);
	glEnable(GL_DEPTH_TEST);
//	_node.draw();
	
//	_node.transformGL();
	ofNode fakeNode;
	ofNotifyEvent(evtDraw3D, fakeNode);
//	_node.restoreTransformGL();
	
	glDisable(GL_DEPTH_TEST);
	
	camera.end();
	

}