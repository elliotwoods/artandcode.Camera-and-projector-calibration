#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	cloudRead = false;
	
	kinect.setUseRegistration(true);
	kinect.init(false);
	kinect.setVerbose(true);
	kinect.open();
	
	recording = false;
	
	recorder.setup();
	recorder.kinect = &kinect;
	recorder.setRecordLocation("pointclouds", "pc_");
	
	
	/*
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
	 */
	
}

//--------------------------------------------------------------

void testApp::update(){
	kinect.update();
	if(kinect.isFrameNew() && recording){
		recorder.addImage( kinect.getRawDepthPixels() );
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	if(recording){
		ofSetColor(255, 0, 0);
		ofRect(0, 0, 640, 10);
	}
	
	ofSetColor(255);
	kinect.draw(0, 10);

	if(cloudRead){
		glEnable(GL_DEPTH_TEST);
		glEnableClientState(GL_VERTEX_ARRAY);
//		glColorPointer(3, GL_FLOAT, sizeof(ofVec3f), &(cloud[0].x));
		glVertexPointer(3, GL_FLOAT, sizeof(Point3f), &(cloud[0].x));
		glDrawArrays(GL_POINTS, 0, cloud.size());
		glDisableClientState(GL_VERTEX_ARRAY);
		
		glDisable(GL_DEPTH_TEST);
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		recording = !recording;
	}

	if(key == 'l'){
		ofDirectory d("pointclouds");
		d.allowExt("pts");
		d.listDir();
		vector<ofFile> files = d.getFiles();
		/*
		for(int i = 0; i < files.size(); i++){
			cout << "loaded file! " << endl;
			points.push_back( recorder.readPointcloud(files[i].getFileName()) );
		}
		 */
		cloudRead = true;
		cloud = recorder.readPointcloud(files[0].getFileName());
		cout << "cloud has " << cloud.size() << " of possible " << 640*480 << endl;
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