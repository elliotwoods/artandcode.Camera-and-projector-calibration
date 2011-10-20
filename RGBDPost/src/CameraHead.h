//
//  CameraHead.h
//  2webcams
//
//  Created by Elliot Woods on 03/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

//#define CAM_RES_X 1280
//#define CAM_RES_Y 720
#define CAM_RES_X 640
#define CAM_RES_Y 360

#define MIN_CALIBS 3

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxCVgui.h"

#include "Ray.h"

using namespace cv;
using namespace ofxCv;


class CameraHead : ofThread {
public:
	CameraHead();
	~CameraHead();
	
	void	init(int iDevice);
	void	close(ofEventArgs& e);
	
	void	update();
	void	drawCorners(ofRectangle &r);
	void	drawOnUndistorted(ofRectangle &r);
	void	drawFrustum(ofNode& n);
	
	void	settings();
	void	load(int i);
	void	save(int i);

	void	add();
	unsigned int	calibCount() { return calib.imagePoints.size(); }
	void			shrink(unsigned int s);
	
	scrGroupGrid	screens;
	scrGroupTabbed	cameraScreen;
	scrDraw2D		cameraRawScreen;
	scrDraw2D		cameraUndistortedScreen;
	scrDraw3D		calibrationScreen;
	
	Calibration	calib;
	
	bool			bangAdded;
	bool			doAdd;
	ofMatrix4x4		matP;
	ofMatrix4x4		matPInverse;
	Ray				ray;
	
	ofEvent<Ray>	evtMoveRay;
	float			viewScale;
protected:
	void	updateCursor(ofVec2f& p);
	
	void	threadedFunction();
	void	threadedAdd();
	
	ofImage		greyBetweenBuffer;
	ofImage		greyThreadedInternal;
	
	float		width, height;
	ofVideoGrabber		video;
	ofImage				videoUndistorted;
	ofImage				liveRGB;
	
	ofMesh				frustum;
	
	vector<Point2f>		imagePoints;
	ofMutex				lockCorners;

};