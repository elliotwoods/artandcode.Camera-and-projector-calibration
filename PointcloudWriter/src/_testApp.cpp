#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(80,80,80);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	//glEnable(GL_DEPTH_TEST);
	
	for (int i=0; i<2; ++i)
		camera[i].init(8+i);
	
	intersecter.color = ofColor(255,255,0);
	
	gui.init(mainScreen);
	mainScreen.setGridWidth(1);

	for (int i=0; i<2; ++i){
		mainScreen.push(camera[i].screens);
	}
	
	ofAddListener(camera[0].calibrationScreen.evtDraw3D, this, &testApp::draw3DCam);
	ofAddListener(camera[1].calibrationScreen.evtDraw3D, this, &testApp::draw3DCam);
	
	ofAddListener(camera[0].evtMoveRay, this, &testApp::updateIntersects);
	ofAddListener(camera[1].evtMoveRay, this, &testApp::updateIntersects);
	
	ofQuickTimeGrabber g;
	g.listDevices();
	
	timerOn = false;
	lastCaptureTime = 0;
	
	ofShortPixels p;
	//fill out p;
	
	ofstream writer;
	writer.open(ofToDataPath("arsewsp").c_str(), ios::binary | ios::out);
	writer.write((char*)p.getPixels(), 2 * 640 * 480);
	writer.close();
	
	ifstream reader;
	reader.open(ofToDataPath("arsewsp").c_str(), ios::binary | ios::in);
	p.allocate(640, 480, OF_PIXELS_MONO);
	reader.read((char*)p.getPixels(), 2 * 640 * 480);
	reader.close();

}

//--------------------------------------------------------------
void testApp::update(){
	for (int i=0; i<2; ++i)
		camera[i].update();
	
	if (camera[0].bangAdded && camera[1].bangAdded) {
		camera[0].bangAdded = false;
		camera[1].bangAdded = false;
		calcTransforms();
	}
	
	if (timerOn)
		if (ofGetElapsedTimef() > (lastCaptureTime+TIMER_PERIOD) && !camera[0].doAdd && !camera[1].doAdd)
		{
			capture();
			lastCaptureTime = ofGetElapsedTimef();
		}
}

//--------------------------------------------------------------
void testApp::draw(){
	
}

void testApp::updateIntersects(Ray &r) {
	
	Ray r1, r2;
	r1 = camera[0].ray;
	r2 = camera[1].ray;
	r2.transform(matPosRotFromXtoOther[1]);
	
	intersecter = r1.intersect(r2);
	
	cout << "intersect length = " << intersecter.t.length() << endl;
	cout << "intersect midpoint = " << intersecter.s + intersecter.t*0.5 << endl;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if (key=='v')
		for(int i=0; i<2; ++i)
			camera[i].settings();
	
	if (key==' ')
	{
		capture();
	}
	
	if (key=='t')
	{
		timerOn ^= true;
		lastCaptureTime = ofGetElapsedTimef();
	}

	if (key=='c')
		clear();
	
	if (key=='s')
		save();
	
	if (key=='l')
		load();
	
}

void testApp::capture() {
	//first check if we've got an asymetry from last time
	int minCalibs = min(camera[0].calibCount(), camera[1].calibCount());
	
	for(int i=0; i<2; ++i)
		camera[i].shrink(minCalibs);
	
	for(int i=0; i<2; ++i)
		camera[i].add();
}

void testApp::calcTransforms() {
	if (camera[0].calib.isReady && camera[1].calib.isReady)
	{
		Mat tra, rot;
		
		if (! (camera[0].calib.getTransformation(camera[1].calib, rot, tra)) ){
			return;
		}
		matPosRotFromXtoOther[0] = makeMatrix(rot, tra);
		
		camera[1].calib.getTransformation(camera[0].calib, rot, tra);
		matPosRotFromXtoOther[1] = makeMatrix(rot, tra);
	}
}

string getAttribName(int iCam, int i, int j) {
	return "Camera" + ofToString(iCam) + "Transform_" + ofToString(i) + "-" + ofToString(j);
}

void testApp::save() {
	FileStorage fs(ofToDataPath("transforms.xml"), FileStorage::WRITE);
	
	for (int iCam=0; iCam<2; iCam++)
	{
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				fs << getAttribName(iCam,i,j) << matPosRotFromXtoOther[iCam](i, j);
		camera[iCam].save(iCam);
	}
}

void testApp::load() {
	FileStorage fs(ofToDataPath("transforms.xml"), FileStorage::READ);
	
	for (int iCam=0; iCam<2; iCam++)
	{
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				fs[getAttribName(iCam,i,j)] >> matPosRotFromXtoOther[i](i, j);
		camera[iCam].load(iCam);
	}
	
	calcTransforms();
}

void testApp::clear() {
	for (int i=0; i<2; ++i)
		camera[i].calib.clean(0);
}

void testApp::draw3DCam(ofNode& n) {
	int i;
	if (&n == (ofNode*)&camera[0].calib)
		i = 0;
	else
		i = 1;
	
	ofDrawAxis(10);
	
	ofPushMatrix();
	ofRotate(90,1,0,0);
	//ofDrawGrid(100);
	ofPopMatrix();
	
	ofPushMatrix();
	glMultMatrixf(matPosRotFromXtoOther[i].getPtr());
	ofDrawAxis(10);
	ofPopMatrix();
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