#include "testApp.h"

testApp::testApp() :
kinectView(kinect),

scrPreviewBoard("Chessboard preview", board),
scrPreviewRGB("RGB camera", kinect.getRGBTextureReference()),
scrKinectView("Kinect", kinectView),
scrControls("Controls"),
scrCVFlags("CV flags"),

wdgScale("Chessboard scale", board.scale, 0, 1.0f, 0.01f),
wdgWhiteBackground("White background", board.whiteBackground),
wdgBrightness("Brightness", board.brightness, 0, 1.0f, 0.01f),
wdgCapture("Capture"),
wdgCursor("World cursor", worldCursor, -3.0f, 3.0f, 0.01f, "m"),
wdgError("Reprojection error", calibrate.error, 0, 200, 0.01f, "pixels"),
wdgClear("Clear")
{
	scrMain.push(scrControls);
	scrMain.push(scrPreviewBoard);
	scrMain.push(scrPreviewRGB);
	scrMain.push(scrKinectView);
	
	scrControls.push(scrCaptureControls);
	scrControls.push(scrCVFlags);
	
	scrCaptureControls.push(wdgScale);
	scrCaptureControls.push(wdgWhiteBackground);
	scrCaptureControls.push(wdgBrightness);
	scrCaptureControls.push(wdgCapture);
	scrCaptureControls.push(wdgCursor);
	scrCaptureControls.push(wdgError);
	scrCaptureControls.push(new wdgCounter("Datapoints", calibrate.count));
	wdgError.enabled = false;
	scrCaptureControls.push(new wdgButton("Reproject chess", showMarkers));
	scrCaptureControls.push(wdgClear);
	scrCaptureControls.push(new wdgButton("Show past finds", showPastFinds));
	
	map<string, bool>::iterator it;
	for (it = calibrate.cvParameters.begin(); it != calibrate.cvParameters.end(); it++) {
		scrCVFlags.push(new wdgButton(it->first, it->second));
	}
	
	wdgCapture.setHotKey(' ');
	wdgClear.setHotKey('c');
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
	ofAddListener(scrPreviewBoard.evtDraw, this, &testApp::drawProjection);
	
	scrKinectView.getNodeReference().pan(180.0f);
	
	showMarkers = true;
	showPastFinds = false;
}

//--------------------------------------------------------------
void testApp::update(){
	
	kinect.update();
	
	board.findCorners(kinect.getRGBPixels(), foundCornersC);	
	kinect.cameraToWorld(foundCornersC, foundCornersW);
	
	if (wdgCapture.getBang())
		capture();
	
	if (wdgClear.getBang())
		calibrate.clear();
	
	if (showMarkers && calibrate.ready) {
		vector<Point3f>::iterator it;
		int iMarker = 0;
		
		vector<Point3f> &xyzMarkers(showPastFinds ? calibrate.xyz : *(vector<Point3f>*)&foundCornersW);
		for (it = xyzMarkers.begin(); it != xyzMarkers.end(); ++it) {
			board.markers[iMarker].enabled = true;
			board.markers[iMarker].xy = calibrate.project(*it) / calibrate.projectorResolution;
			if (++iMarker >= MAX_MARKERS)
				break;
		}
	} else {
		for (int i=0; i<MAX_MARKERS; ++i) {
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
	calibrate.draw();	
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
void testApp::drawProjection(ofRectangle &r) {
	
	ofPushStyle();
	ofNoFill();
	
	Point2f xyProjector, xyInView;
	for (int i=0; i<calibrate.xy.size(); i++) {
		xyProjector = calibrate.xy[i];
		xyInView.x = r.x + r.width * xyProjector.x / calibrate.projectorResolution.x;
		xyInView.y = r.y + r.height * xyProjector.y / calibrate.projectorResolution.y;
		ofCircle(xyInView.x,xyInView.y , 10);
	}
	
	ofPopStyle();
//	
//	ofPushView();
//	
//	calibrate.intrinsics.loadProjectionMatrix(0.1);
//	
//	for (int i=0; i<foundCornersW.size(); i++) {
//		ofPushMatrix();
//		ofTranslate(foundCornersW[i]);
//		ofSphere(0.01);
//		ofPopMatrix();
//	}
//	
//	ofPopView();
}

//--------------------------------------------------------------
void testApp::capture() {
	projectedCornersP = board.getProjectionSpaceCorners();
	
	for (int i=0; i<foundCornersC.size(); ++i)
		if (foundCornersW[i].length() > 0.1f)
			calibrate.push(foundCornersW[i], projectedCornersP[i]);
	
	calibrate.correlate();
}
