#pragma once

#define TIMER_PERIOD 5.0f

#include "ofMain.h"

#include "ofxCVgui.h"

#include "CameraHead.h"
#include "ray.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	CameraHead		camera[2];
	
	
	Ray intersecter;
	ofMatrix4x4		matPosRotFromXtoOther[2];
	
	void capture();

	void calcTransforms();
	void updateIntersects(Ray &r);
	
	void clear();
	void load();
	void save();
	
	void draw3DCam(ofNode &n);
	
private:
	scrGroupGrid	mainScreen;
	ofxCVgui		gui;
	
	bool		timerOn;
	float		lastCaptureTime;
};
