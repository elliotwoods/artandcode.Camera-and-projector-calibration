#include "testApp.h"

testApp::testApp()
: kinectView("Kinect View", kinect.getTextureReference()),
  cameraView("Color View", grayCopy)

{
	
}

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	kinect.init(true, true, true);
	kinect.open();
	
	rgbcamera.setDeviceID(8);
	rgbcamera.initGrabber(640, 480);
	
	kinectCheckerPreview.setup(10,7,4);
	cameraCheckerPreview.setup(10,7,4);
	
	depthRGBAlignment.setup();
	
	gui.init(mainScreen);
	mainScreen.push(kinectView);
	mainScreen.push(cameraView);
	
	ofAddListener(kinectView.evtDraw, this, &testApp::drawOnKinect);
	ofAddListener(cameraView.evtDraw, this, &testApp::drawOnCamera);
}

//--------------------------------------------------------------
void testApp::update(){
	
	bool isupdated = false;
	kinect.update();
	if(kinect.isFrameNew()){
//		depthRGBAlignment.setDepthImage( kinect.getRawDepthPixels() );
		kinectCheckerPreview.setTestImage(kinect.getPixelsRef());
		isupdated = true;
	}
	
	rgbcamera.update();
	if(rgbcamera.isFrameNew()){
//		depthRGBAlignment.setColorImage( rgbcamera.getPixelsRef() );
		grayCopy.setFromPixels(rgbcamera.getPixelsRef());
		grayCopy.setImageType(OF_IMAGE_GRAYSCALE);
		cameraCheckerPreview.setTestImage( grayCopy.getPixelsRef() );		
		isupdated = true;
	}
	
	if(isupdated){
	//	depthRGBAlignment.update();
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	depthRGBAlignment.drawPointCloud();
//	grayCopy.draw(640, 0);
//	rgbCheckerPreview.draw(ofVec2f(640,0));
}

void testApp::drawOnKinect(ofRectangle& drawRect){
//	kinect.draw(0, 0);
	kinectCheckerPreview.draw(drawRect);	
}

void testApp::drawOnCamera(ofRectangle& drawRect){
	cameraCheckerPreview.draw(drawRect);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
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