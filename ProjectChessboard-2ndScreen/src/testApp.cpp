#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	network.init();
	ofSetFrameRate(20.0f);
	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void testApp::update(){
	
	TalkyMessage msg;
	while (network >> msg) {
		if (msg.getPayloadLength() == sizeof(Chessboard))
			msg >> board;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	board.draw(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key=='f')
		ofToggleFullscreen();
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