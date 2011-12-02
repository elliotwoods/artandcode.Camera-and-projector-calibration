#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	camWidth 		= 720;	// try to grab at this size. 
	camHeight 		= 576;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
	
	fullscreen = false;
}


//--------------------------------------------------------------
void testApp::update(){
		vidGrabber.grabFrame();

}

//--------------------------------------------------------------
void testApp::draw(){
	if (fullscreen)
		vidGrabber.draw(ofGetWidth(),0,-ofGetWidth(), ofGetHeight());
	else
		vidGrabber.draw(0,0);
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	if (key == 's' || key == 'S'){
		vidGrabber.videoSettings();
	}
	
	if (key == 'f') {
		fullscreen ^= true;
		ofSetFullscreen(fullscreen);
	}
	
	
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
