//
//  CameraHead.cpp
//  2webcams
//
//  Created by Elliot Woods on 03/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//


#include "CameraHead.h"

CameraHead::CameraHead() :
cameraRawScreen("Raw", video),
cameraUndistortedScreen("Unidisorted", videoUndistorted),
cameraScreen("Camera"),
calibrationScreen("Calibration", calib),
doAdd(false), bangAdded(false),
viewScale(1.85f) // TODO : find where this comes from. this value is for a 640x360 camera at 1 calibration and at one position. should be used as an order of magnitude to find true evaluatable value
{
	cameraScreen.push(cameraRawScreen);
	cameraScreen.push(cameraUndistortedScreen);
	
	calibrationScreen.setLock(lockCorners);
	
	screens.push(cameraScreen);
	screens.push(calibrationScreen);
	ofAddListener(cameraRawScreen.evtDraw, this, &CameraHead::drawCorners);
	ofAddListener(cameraUndistortedScreen.evtDraw, this, &CameraHead::drawOnUndistorted);
	ofAddListener(calibrationScreen.evtDraw3D, this, &CameraHead::drawFrustum);
	ofAddListener(cameraUndistortedScreen.evtMousePressed, this, &CameraHead::updateCursor);
	
	ofAddListener(ofEvents.exit, this, &CameraHead::close);
	
	ray.s = ofVec3f(0,0,0);
	ray.t = ofVec3f(0,0,1);
	ray.color = ofColor(255,40, 150);
	ray.infinite = true;
	
	//JG couldn't compile this...
	//calib.setSquarePixelSize(width / 60);

}

CameraHead::~CameraHead() {
	ofEventArgs e;
	close(e);
}

void CameraHead::init(int iDevice) {
	video.setDeviceID(iDevice);
	video.initGrabber(CAM_RES_X, CAM_RES_Y);
	imitate(videoUndistorted, video);
	
	width = video.getWidth();
	height = video.getHeight();
	
	liveRGB.allocate(width, height, OF_IMAGE_COLOR);
	greyBetweenBuffer.allocate(width, height, OF_IMAGE_GRAYSCALE);
	greyThreadedInternal.allocate(width, height, OF_IMAGE_GRAYSCALE);
	
	calib.setPatternSize(10,7);
	calib.setSquareSize(4);
	
	frustum.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	frustum.addVertex(ofVec3f(0, 0, 0));
	frustum.addVertex(ofVec3f(width, 0, 0));
	frustum.addVertex(ofVec3f(width, height, 0));
	frustum.addVertex(ofVec3f(0, height, 0));
	frustum.addVertex(ofVec3f(0, 0, 0));
	
	frustum.addVertex(ofVec3f(0, 0, 100));
	frustum.addVertex(ofVec3f(width, 0, 100));
	frustum.addVertex(ofVec3f(width, height, 100));
	frustum.addVertex(ofVec3f(0, height, 100));
	frustum.addVertex(ofVec3f(0, 0, 100));
	
	frustum.addVertex(ofVec3f(width, 0, 100));
	frustum.addVertex(ofVec3f(width, 0, 0));
	frustum.addVertex(ofVec3f(width, height, 0));
	frustum.addVertex(ofVec3f(width, height, 100));
	frustum.addVertex(ofVec3f(0, height, 100));
	frustum.addVertex(ofVec3f(0, height, 0));
	
	startThread(true, false);
}

void CameraHead::close(ofEventArgs& e) {
	waitForThread(true);
}

void CameraHead::update () {	
	video.update();
	liveRGB.setFromPixels(video.getPixelsRef());
	
	//copy to mid buffer
	lock();
	unsigned char *in = liveRGB.getPixels();
	unsigned char *out = greyBetweenBuffer.getPixels();

	for (int i=0; i<width*height; ++i)
	{
		*out = *in/3 + *(in+1)/3 + *(in+2)/3;
		
		in+=3;
		++out;
	}
	//undistort
	if (calib.isReady && cameraScreen.iSelection==1)
	{
		calib.undistort(toCv(video), toCv(videoUndistorted));
		videoUndistorted.update();
	}
	unlock();
}

void CameraHead::drawCorners(ofRectangle &viewport) {
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(3);
	ofSetColor(255,100,30);
	
	if (lockCorners.tryLock()) {
		float x, y;
		for (int i=0; i<imagePoints.size(); ++i)
		{
			x = imagePoints[i].x / width * viewport.width + viewport.x;
			y = imagePoints[i].y / height * viewport.height + viewport.y;

			ofCircle(x, y, 5);
		}
		lockCorners.unlock();
	}
	ofPopStyle();

	drawHighlightString("isReady = " + string(calib.isReady ? "true" : "false"), viewport.x+5, viewport.y + viewport.height-60, ofColor(200,100,100), ofColor(255,255,255));
	drawHighlightString("Calibration sets = " + ofToString(calib.imagePoints.size()), viewport.x+5, viewport.y + viewport.height-40, ofColor(200,100,100), ofColor(255,255,255));
	drawHighlightString("Found corners = " + ofToString(imagePoints.size()), viewport.x+5, viewport.y + viewport.height-20, ofColor(200,100,100), ofColor(255,255,255));
}

void CameraHead::drawFrustum(ofNode& n) {
	if (!calib.isReady)
		return;
	
	ofPushMatrix();
	glMultMatrixf(matPInverse.getPtr());
	frustum.draw();	
	ofPopMatrix();
	
	ray.draw(5);
}

void CameraHead::drawOnUndistorted(ofRectangle &r) {
	
	if (!calib.isReady)
		return;
	
	ofPushView();
	ofViewport(r);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(matP.getPtr());
	glScalef(viewScale, -viewScale*640/360, 1.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (lockCorners.tryLock())
	{
		calib.draw3d();
		lockCorners.unlock();
	}
	
	ofPopView();
}

void CameraHead::settings() {
	video.videoSettings();
}

void CameraHead::load(int i) {
	lock();
	calib.load("calib-" + ofToString(i) + ".yml");
	unlock();
}

void CameraHead::save(int i) {
	lock();
	calib.save("calib-" + ofToString(i) + ".yml");
	unlock();
}

void CameraHead::add() {
	lock();
	doAdd = true;
	unlock();
}

void CameraHead::threadedFunction() {
	
	while (isThreadRunning())
	{
		lock();
		memcpy(greyThreadedInternal.getPixels(), greyBetweenBuffer.getPixels(), width*height);
		unlock();
		
		if (doAdd)
			threadedAdd();
		else {
			vector<Point2f> points;
			bool found = calib.findBoard(toCv(greyThreadedInternal), points);
			
			if (found){
				lock();
				imagePoints = points;
				unlock();
			}
		}
	}	
}

void CameraHead::threadedAdd() {
	if (lockCorners.tryLock())
	{
		calib.add(toCv(greyThreadedInternal));
		doAdd = false;
		
		if (calibCount() >= MIN_CALIBS)
			calib.calibrate();
		
		if (calib.isReady) {
			Mat matCv = calib.getUndistortedIntrinsics().getCameraMatrix();
			ofMatrix4x4 matC;

			
			
			//////////
			//forwards
			//////////
			//
			matC.makeIdentityMatrix();
			for (int i=0; i<3; ++i)
				for (int j=0; j<3; ++j)
					matC(i<2?i:3,j<2?j:3) = matCv.at<double>(j, i);
			
			matP.makeIdentityMatrix();
			
			matP(3,3) = 0; //confirmed
			matP(2,3) = 1;
			
			cout << "Camera matrix:\n" << matC << endl;
			cout << "Projection  matrix:\n" << matP << endl;
			matP = matP * matC; //confirmed
			cout << "Combined matrix:\n" << matP << endl << "--" << endl;
			//
			//////////
			
			
			
			//////////
			//inverse
			//////////
			//
			Mat matCvInv = matCv.inv(DECOMP_SVD);
			matC.makeIdentityMatrix();
			for (int i=0; i<3; ++i)
				for (int j=0; j<3; ++j)
					matC(i<2?i:3,j<2?j:3) = matCvInv.at<double>(j, i);
			matP.makeIdentityMatrix();
			matP(3,3) = 0; //confirmed
			matP(2,3) = 1;
			
			matPInverse = matC * matP.getInverse();
			//
			//////////
			
		}
		
		bangAdded = true;
		lockCorners.unlock();
	}
}

void CameraHead::shrink(unsigned int s) {
	while (calibCount() > s)
		calib.imagePoints.erase(calib.imagePoints.end()-1);
}

void CameraHead::updateCursor(ofVec2f& p) {
	if (lockCorners.tryLock())
	{
		if (calib.isReady)
		{
			ofVec3f c;
			ofRectangle scr = cameraUndistortedScreen.getLiveBounds();
			c.x = p.x * width;
			c.y = p.y * height;
			c.z = 100.0f;
			ray.t = c * matPInverse;
				
			cout << "ray look vector: " << ray.t << endl;
			
			ofNotifyEvent(evtMoveRay, ray, this);
		}
		lockCorners.unlock();
	}
}