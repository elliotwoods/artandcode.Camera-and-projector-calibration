#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	kinect.init(true);
	kinect.setVerbose(true);
	kinect.open();

	slr.setup();
//	rgbcamera.setDeviceID(8);
//	rgbcamera.initGrabber(640, 480);
	
	kinectCheckerPreview.setup(10,7,4);
	cameraCheckerPreview.setup(10,7,4);
	depthRGBAlignment.setup(10,7,4);
	
	kinectView = new scrDraw2D("Kinect View", kinect.getTextureReference());
	cameraView = new scrDraw2D("Camera View", grayCopy);
	depthView  = new scrDraw2D("Depth View", kinect.getDepthTextureReference());
	pointcloudView = new scrGameView3D("Point Cloud", "PointCamera.xml");
	
	mainScreen = new scrGroupTabbed("", 32);
	
	calibrate = new scrGroupGrid("Calibrate");
	calibrate->push(kinectView);
	calibrate->push(cameraView);
	calibrate->push(depthView);
	
	preview = new scrGroupGrid("Preview");
	preview->push(pointcloudView);
	
	mainScreen->push(calibrate);
	mainScreen->push(preview);
	
	ofAddListener(kinectView->evtDraw, this, &testApp::drawOnKinect);
	ofAddListener(cameraView->evtDraw, this, &testApp::drawOnCamera);
	ofAddListener(pointcloudView->evtDraw3D, this, &testApp::drawOnPoint);
	gui = new ofxCVgui();
	gui->init(*mainScreen);

	
}

//--------------------------------------------------------------
void testApp::update(){
	
	bool isupdated = false;
	kinect.update();
	if(kinect.isFrameNew()){
		if(mainScreen->iSelection == 0){
			kinectCheckerPreview.setTestImage(kinect.getPixelsRef());
		}
		else {
			depthRGBAlignment.setDepthImage( kinect.getRawDepthPixels() );		
			/*
			vector<Point3f> newpoints;
			for(int y = 0; y < kinect.getHeight(); y++){
				for(int x = 0; x < kinect.getWidth(); x++){
					newpoints.push_back(toCv(kinect.getWorldCoordinateAt(x, y)));
				}
			}
			depthRGBAlignment.setPoinCloud(newpoints);
			 */
		}
		isupdated = true;
	}

	/*
	rgbcamera.update();
	if(rgbcamera.isFrameNew()){
		depthRGBAlignment.setColorImage( rgbcamera.getPixelsRef() );
		grayCopy.setFromPixels(rgbcamera.getPixelsRef());
		grayCopy.setImageType(OF_IMAGE_GRAYSCALE);
		cameraCheckerPreview.setTestImage( grayCopy.getPixelsRef() );		
		isupdated = true;
	}
	*/
	
	slr.update();
	if(slr.isFrameNew()) {
//		cout << " new frame" << endl;
		if(mainScreen->iSelection == 0){
			grayCopy.setFromPixels( slr.getLivePixels() );
			grayCopy.setImageType(OF_IMAGE_GRAYSCALE);
			cameraCheckerPreview.setTestImage( grayCopy.getPixelsRef() );		
		}
		else{
			depthRGBAlignment.setColorImage( slr.getLivePixels() );
		}
		isupdated = true;		
	}
	
	if(isupdated && mainScreen->iSelection == 1){
		depthRGBAlignment.update();
		pointcloudNode.setPosition(depthRGBAlignment.getMeshCenter());
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
}

void testApp::drawOnKinect(ofRectangle& drawRect){
	kinectCheckerPreview.draw(drawRect);	
}

void testApp::drawOnCamera(ofRectangle& drawRect){
	cameraCheckerPreview.draw(drawRect);
}

void testApp::drawOnPoint(ofNode& drawNode){
	depthRGBAlignment.drawPointCloud();
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

void testApp::exit() {
	kinect.close();
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