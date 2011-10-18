#include "testApp.h"

testApp::testApp() :
kinectView(kinect),

scrPreviewBoard("Chessboard preview", board),
scrPreviewRGB("RGB camera", kinect.getRGBTextureReference()),
scrKinectView("Kinect", kinectView),

wdgScale("Chessboard scale", board.scale, 0, 1.0f, 0.01f),
wdgWhiteBackground("White background", board.whiteBackground),
wdgBrightness("Brightness", board.brightness, 0, 1.0f, 0.01f),
wdgCapture("Capture"),
wdgCursor("World cursor", worldCursor, -3.0f, 3.0f, 0.01f, "m"),
wdgError("Reprojection error", correlation.error, 0, 200, 0.01f, "pixels") {
	scrMain.push(scrControl);
	scrMain.push(scrPreviewBoard);
	scrMain.push(scrPreviewRGB);
	scrMain.push(scrKinectView);
	
	scrControl.push(wdgScale);
	scrControl.push(wdgWhiteBackground);
	scrControl.push(wdgBrightness);
	scrControl.push(wdgCapture);
	scrControl.push(wdgCursor);
	scrControl.push(wdgError);
	scrControl.push(new wdgCounter("Datapoints", correlation.count));
	wdgError.enabled = false;
	scrControl.push(new wdgButton("Reproject chess", showMarkers));

	showMarkers = false;
	
	wdgCapture.setHotKey(' ');
}

//--------------------------------------------------------------
void testApp::setup(){
	
	screens.init(scrMain);	
	ofBackground(117/2,130/2,160/2);
	ofSetFrameRate(60.0f);
	glEnable(GL_DEPTH_TEST);
	
	client.init("localhost");
	
	kinect.setupFromXML("openni/config/ofxopenni_config.xml",false);
	kinect.enableCalibratedRGBDepth();
	
	ofAddListener(scrPreviewRGB.evtDraw, this, &testApp::drawFoundCorners2D);
	ofAddListener(scrKinectView.evtDraw3D, this, &testApp::drawFoundCorners3D);
	ofAddListener(scrPreviewRGB.evtMousePressed, this, &testApp::pipetRGB);
	
	scrKinectView.getNodeReference().pan(180.0f);
}

//--------------------------------------------------------------
void testApp::update(){
	
	kinect.update();
	
	board.findCorners(kinect.getRGBPixels(), foundCornersC);	
	kinect.cameraToWorld(foundCornersC, foundCornersW);
	
	if (wdgCapture.getBang())
		capture();
	
	if (showMarkers) {
		vector<ofVec3f>::iterator it;
		int iMarker = 0;
		for (it = foundCornersW.begin(); it != foundCornersW.end(); ++it) {
			board.markers[iMarker].enabled = true;
			board.markers[iMarker].xy = correlation.project(*it);
			if (iMarker++ >= 10)
				break;
		}
	} else {
		for (int i=0; i<10; ++i) {
			board.markers[i].enabled = false;
		}
	}
	
	if (wdgScale.isValueNew() || wdgWhiteBackground.isValueNew() || wdgBrightness.isValueNew() || showMarkers || ofGetFrameNum() % 10 == 0) {
		TalkyMessage msg;
		msg << board;
		client << msg;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::pipetRGB(ofVec2f &m){
	ofVec2f c;
	
	c.x = m.x * 640;
	c.y = m.y * 480;
	worldCursor = kinect.cameraToWorld(c);
}

//--------------------------------------------------------------
void testApp::drawFoundCorners2D(ofRectangle &r){
	ofPushStyle();
	ofEnableSmoothing();
	ofNoFill();
	ofSetLineWidth(3.0f);
	ofSetColor(200,100,50);
	
	ofBeginShape();
	ofVec2f c;
	vector<ofVec2f>::iterator it;
	for (it = foundCornersC.begin(); it!= foundCornersC.end(); ++it) {
		c.x = it->x / 640.f * r.width + r.x;
		c.y = it->y / 480.f * r.height + r.y;
		ofCircle(c.x, c.y, 5);
		ofVertex(c.x, c.y);
	}
	
	ofSetColor(255,100,50);
	ofSetLineWidth(2.0f);
	ofEndShape(false);
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::drawFoundCorners3D(ofNode &n){
	
	//////////////
	// Current
	//////////////
	//
	ofPushStyle();
	ofEnableSmoothing();
	ofSetColor(200,100,50);
	
	vector<ofVec3f>::iterator it;
	for (it = foundCornersW.begin(); it != foundCornersW.end(); ++it) {
		ofPushMatrix();
		ofTranslate(*it);
		ofBox(0.01);
		ofPopMatrix();
	}
	
	ofPopStyle();
	//
	//////////////
	
	
	//////////////
	// Recorded
	//////////////
	//
	ofPushStyle();
	correlation.draw();	
	ofPopStyle();
	//
	//////////////
	
	//////////////
	// Cursor
	//////////////
	//
	ofPushMatrix();
	ofPushStyle();
	
	ofTranslate(worldCursor);
	
	ofSetColor(255, 50, 50);
	ofSphere(0.02);
	
	ofPopStyle();
	ofPopMatrix();
	//
	//////////////
}

//--------------------------------------------------------------
void testApp::capture() {
	projectedCornersP = board.getProjectionSpaceCorners();
	
	for (int i=0; i<foundCornersC.size(); ++i)
		if (foundCornersW[i].length() > 0.5f)
			correlation.push(foundCornersW[i], projectedCornersP[i]);
	
	correlation.correlate();
}
