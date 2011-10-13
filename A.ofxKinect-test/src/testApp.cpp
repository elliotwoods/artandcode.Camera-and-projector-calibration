#include "testApp.h"

testApp::testApp() :
kinectView(kinect),

scrPreviewDepth("Depth", kinect.getDepthTextureReference()),
scrPreviewRGB("RGB", kinect.getTextureReference()),
scr3D("3D", kinectView),
wdgTilt("Kinect tilt", tilt, -30, 30)

{
	scrMain.push(scrPreviewDepth);
	scrMain.push(scrPreviewRGB);
	scrMain.push(scrControl);
	scrMain.push(scr3D);
	
	scrControl.push(wdgTilt);
}

//--------------------------------------------------------------
void testApp::setup(){	
	screens.init(scrMain);	
	ofBackground(117/2,130/2,160/2);
	
	kinect.init();
	kinect.open();
	
	tilt = kinect.getTargetCameraTiltAngle();
}

//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	
	if (wdgTilt.isValueNew())
		kinect.setCameraTiltAngle(tilt);
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}