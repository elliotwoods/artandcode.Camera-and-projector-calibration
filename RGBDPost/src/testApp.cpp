#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	kinect.setUseRegistration(true);
	kinect.init(false);
	kinect.setVerbose(true);
	kinect.open();

	slr.setup();
//	rgbcamera.setDeviceID(8);
//	rgbcamera.initGrabber(640, 480);
	
	kinectCheckerPreview.setup(10,7,4);
	cameraCheckerPreview.setup(10,7,4);
	depthRGBAlignment.setup(10,7,4);
	
	grayscaleExternalCamera.allocate(10, 10, OF_IMAGE_GRAYSCALE);
	grayscaleKinectCamera.allocate(10, 10, OF_IMAGE_GRAYSCALE);
	
	kinectView = new scrDraw2D("Kinect View", grayscaleKinectCamera);
	cameraView = new scrDraw2D("Camera View", grayscaleExternalCamera);
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

void testApp::calibrateFromDirectory(){
	ofFileDialogResult aResults = ofSystemLoadDialog("Load Depth Perspective Chessboards", true);
	if(aResults.bSuccess){
		ofFileDialogResult bResults = ofSystemLoadDialog("Load External Images Chessboards", true);
		if(bResults.bSuccess){
			alignment.calibrateFromDirectoryPair(aResults.getPath(), bResults.getPath());
		}
	}
}

void testApp::loadPreviewSequence(){
	ofFileDialogResult aResults = ofSystemLoadDialog("Load Depth Images", true);
	if(aResults.bSuccess){
		ofFileDialogResult bResults = ofSystemLoadDialog("Load External Images", true);
		if(bResults.bSuccess){
			
			ofDirectory dirList;
			dirList.listDir(directory);
			for(int i = 0; i < dirList.size(); i++) {
				cur.loadImage(dirList.getPath(i));
				if(!add(toCv(cur))) {
					ofLog(OF_LOG_ERROR, "Calibration::add() failed on " + dirList.getPath(i));
				}
			}
			
			alignment.calibrateFromDirectoryPair(aResults.getPath(), bResults.getPath());
			framesLoaded = true;
		}
	}
	
}

//--------------------------------------------------------------
void testApp::update(){

	bool isupdated = false;
	
	slr.update();
	if(slr.isFrameNew()) {
		
		if(mainScreen->iSelection == 0){
			grayscaleExternalCamera.setFromPixels( slr.getLivePixels() );
			grayscaleExternalCamera.setImageType(OF_IMAGE_GRAYSCALE);
			cameraCheckerPreview.setTestImage( grayscaleExternalCamera.getPixelsRef() );		
		}
		else{
			ofPixelsRef pix = slr.getLivePixels();
			colorExternalCamera.setFromPixels(pix.getPixels(), pix.getWidth(), pix.getHeight(), OF_IMAGE_COLOR);
			depthRGBAlignment.setColorImage( colorExternalCamera );
		}
		isupdated = true;		
	}
	
	kinect.update();
	if(kinect.isFrameNew()){
		if(mainScreen->iSelection == 0){
			ofPixelsRef p = kinect.getPixelsRef();
//			cout << " kinect size is " << p.getWidth() << " " << p.getHeight() << endl;
			grayscaleKinectCamera.setFromPixels( kinect.getPixels(), kinect.getWidth(), kinect.getHeight(), OF_IMAGE_COLOR );
			grayscaleKinectCamera.setImageType(OF_IMAGE_GRAYSCALE);
			kinectCheckerPreview.setTestImage(grayscaleKinectCamera.getPixelsRef());
		}
		else {
			//depthRGBAlignment.setDepthImage( kinect.getRawDepthPixels() );
			vector<Point3f> newpoints;
			for(int y = 0; y < kinect.getHeight(); y++){
				for(int x = 0; x < kinect.getWidth(); x++){
					float color = kinect.getPixelsRef().getColor(x, y).getBrightness();
					if(color != 0){
						ofVec3f worldp = kinect.getWorldCoordinateAt(x, y);
						newpoints.push_back(toCv(worldp));
					}
					else{
						newpoints.push_back(toCv(ofVec3f(0,0,0)));
					}
				}
			}
			depthRGBAlignment.setPointCloud(newpoints);
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
	
	if(isupdated && mainScreen->iSelection == 1){
		depthRGBAlignment.update();
		//pointcloudNode.setPosition(depthRGBAlignment.getMeshCenter());
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
//	slr.update();
//	slr.draw(0, 0);

}

void testApp::drawOnKinect(ofRectangle& drawRect){
	kinectCheckerPreview.draw(drawRect);	
}

void testApp::drawOnCamera(ofRectangle& drawRect){
	cameraCheckerPreview.draw(drawRect);
}

void testApp::drawOnPoint(ofNode& drawNode){
	depthRGBAlignment.drawPointCloud();
	//depthRGBAlignment.drawMesh();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		depthRGBAlignment.addCalibrationImagePair(grayscaleKinectCamera.getPixelsRef(), grayscaleExternalCamera.getPixelsRef());
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