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
	camera.setNearClip(0.01f);
	camera.speed = 15;
	camera.autosavePosition = true;
	camera.useArrowKeys = false;
	camera.cameraPositionFile = cameraPositionFile;
	camera.loadCameraPosition();
}

void scrGameView3D::drawContent()
{
	ofPushStyle();
	ofSetColor(0);

	ofRectangle viewport = getLiveBounds();
	ofRect(	viewport );
	ofPopStyle();
	
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