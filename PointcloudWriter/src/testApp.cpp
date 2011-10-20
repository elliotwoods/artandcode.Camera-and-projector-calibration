#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	frameRead = false;
	
	kinect.setUseRegistration(true);
	kinect.init(false);
	kinect.setVerbose(true);
	kinect.open();
	
	recording = false;
	

    recorder.setRecordLocation("depthframes", "frame_");
    recorder.setup();
	
	cam.speed = 5;

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

	if(frameRead){
		cam.begin();
		int currentFrame = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, clouds.size(), true);
		glEnable(GL_DEPTH_TEST);
		glEnableClientState(GL_VERTEX_ARRAY);
//		glColorPointer(3, GL_FLOAT, sizeof(ofVec3f), &(cloud[0].x));
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &(clouds[currentFrame][0].x));
		glDrawArrays(GL_POINTS, 0, clouds[currentFrame].size());
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_DEPTH_TEST);
		
		cam.end();
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		recording = !recording;
        if(!recording){
            recorder.incrementFolder();
        }
	}

	if(key == 'l'){
		ofDirectory d("depthframes");
		d.allowExt("xkcd");
		int numfiles = d.listDir();
		frameRead = true;
		for(int f = 0; f < 30; f++){
//		for(int f = 0; f < 1; f++){			
			vector<ofVec3f> cloud;
			unsigned short* frame = recorder.readDepthFrame( "depthframes/" + d.getName(f) );
			for(int y = 0; y < 480; y++){
				for(int x = 0; x < 640; x++){
					//cout << frame[y*640+x] << endl;
					ofVec3f v = kinect.getWorldCoordinateAt(x, y, frame[y*640+x] );
					cloud.push_back( v );
				}
			}
			cout << "read frame depthframes/" <<d.getName(f) << endl;
			delete frame;
			clouds.push_back( cloud );
		}
	}
    
    if(key == 'c'){
        string filename = "__CalibFile_" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" + ofToString(ofGetSeconds()) +".png";
        ofImage kinectImage;
        kinectImage.setFromPixels(kinect.getPixels(), 640, 480, OF_IMAGE_COLOR);
        ofSaveImage( kinectImage, filename);
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