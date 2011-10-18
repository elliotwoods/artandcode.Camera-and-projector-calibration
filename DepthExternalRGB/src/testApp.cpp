#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	kinect.init(true, true, true);
	kinect.open();
	

	rgbcamera.setDeviceID(8);
	rgbcamera.initGrabber(640, 480);
	
	kinectCheckerPreview.setup();
	rgbCheckerPreview.setup();
	
	depthRGBAlignment.setup();
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
		rgbCheckerPreview.setTestImage( grayCopy.getPixelsRef() );
		isupdated = true;
	}
	
	if(isupdated){
	//	depthRGBAlignment.update();
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	depthRGBAlignment.drawPointCloud();
	kinect.draw(0, 0);
	grayCopy.draw(640, 0);
	kinectCheckerPreview.draw(ofVec2f(0,0));
	rgbCheckerPreview.draw(ofVec2f(640,0));
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