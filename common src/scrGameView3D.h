/*
 *  scrGameView3D.h
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/19/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxGameCamera.h"
#include "ofxCVgui.h"

class scrGameView3D : public scrBase
{
  public:
	scrGameView3D(string caption, string cameraPositionFile);
	
	void			keyPressed(ofKeyEventArgs & args);
//	ofNode&			getNodeReference();
	
	float			pointSize;
	ofEvent<ofNode>	evtDraw3D; ///< differs from standard event, in that it is triggered after the transforms have been applied
	
	
protected:
	virtual void	drawContent();
		
	ofxGameCamera	camera;
};