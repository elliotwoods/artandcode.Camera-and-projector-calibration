#include "testApp.h"

//testApp::testApp()
//{
//	
//}

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	
	rgbcamera.setDeviceID(8);
	rgbcamera.initGrabber(640, 480);
	
	kinectCheckerPreview.setup(10,7,4);
	cameraCheckerPreview.setup(10,7,4);
	
	kinectView = new scrDraw2D("Kinect View", kinect.getTextureReference());
	cameraView = new scrDraw2D("Camera View", grayCopy);
	pointcloudView = new scrDraw3D("Point Cloud", pointcloudNode);
	
	depthRGBAlignment.setup(10,7,4);
	mainScreen = new scrGroupGrid();
	mainScreen->push(kinectView);
	mainScreen->push(cameraView);
	mainScreen->push(pointcloudView);
	gui.init(*mainScreen);
	
	ofAddListener(kinectView->evtDraw, this, &testApp::drawOnKinect);
	ofAddListener(cameraView->evtDraw, this, &testApp::drawOnCamera);
	ofAddListener(pointcloudView->evtDraw, this, &testApp::drawOnPoint);
	
	kinect.init(true, true, true);	
	kinect.open();
	
	gamecam.speed = 5;
	gamecam.usemouse = true;
	gamecam.autosavePosition = true;

	//gamecam.setScale(ofVec3f(1,-1,1));
	gamecam.loadCameraPosition();
	
}

//--------------------------------------------------------------
void testApp::update(){
	bool isupdated = false;
	kinect.update();
	if(kinect.isFrameNew()){
		depthRGBAlignment.setDepthImage( kinect.getRawDepthPixels() );
		kinectCheckerPreview.setTestImage(kinect.getPixelsRef());
		isupdated = true;
	}
	
	rgbcamera.update();
	if(rgbcamera.isFrameNew()){
		depthRGBAlignment.setColorImage( rgbcamera.getPixelsRef() );
		grayCopy.setFromPixels(rgbcamera.getPixelsRef());
		grayCopy.setImageType(OF_IMAGE_GRAYSCALE);
		cameraCheckerPreview.setTestImage( grayCopy.getPixelsRef() );		
		isupdated = true;
	}
	
	if(isupdated){
		depthRGBAlignment.update();
		pointcloudNode.setPosition(depthRGBAlignment.getMeshCenter());
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	//cam.setPosition(depthRGBAlignment.getMeshCenter() + ofVec3f(0,0,1) * (depthRGBAlignment.getMeshDistance() + ofGetMouseX()) );
	//cam.lookAt(depthRGBAlignment.getMeshCenter(), ofVec3f(0,1,0));
}

void testApp::drawOnKinect(ofRectangle& drawRect){
	kinectCheckerPreview.draw(drawRect);	
}

void testApp::drawOnCamera(ofRectangle& drawRect){
	cameraCheckerPreview.draw(drawRect);
}

void testApp::drawOnPoint(ofRectangle& drawRect){
	//	cam.begin();
	//gamecam.begin(drawRect);
	
	depthRGBAlignment.drawPointCloud();
	ofPushStyle();
	ofSetColor(255, 0, 0);
	ofBox(pointcloudNode.getPosition(), 2);
	ofPopStyle();
	
	//cam.end();
	//gamecam.end();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		depthRGBAlignment.addCalibrationImagePair(kinect.getPixelsRef(), grayCopy.getPixelsRef());
	}
	
	if(key == 's'){
		depthRGBAlignment.saveCalibration();
	}
	
	if(key == 'l'){
		depthRGBAlignment.loadCalibration();		
	}
	
	if(key == 'v'){
		rgbcamera.videoSettings();
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