#include "testApp.h"

testApp::testApp(){

}

//--------------------------------------------------------------
void testApp::setup(){
	xyz.push_back(ofVec3f(0.4245337450, -0.3661487279, -0.4878357339));
	xyz.push_back(ofVec3f(0.3506861092, 0.4697300799, 0.4053756632));
	xyz.push_back(ofVec3f(-0.4230080058, 0.1083145491, 0.4126259645));
	xyz.push_back(ofVec3f(-0.1698530512, -0.0650750778, 0.4814476264));
	xyz.push_back(ofVec3f(0.2610632244, 0.3133410532, 0.1916766560));
	xyz.push_back(ofVec3f(0.1478232923, 0.3995812254, -0.4930971479));
	xyz.push_back(ofVec3f(0.0032118086, 0.1184657835, 0.2893070101));
	xyz.push_back(ofVec3f(0.4964968562, 0.4479994180, -0.2135472880));
	xyz.push_back(ofVec3f(-0.3507092523, 0.3956844574, 0.4353991777));
	xyz.push_back(ofVec3f(0.1563462173, -0.4588385050, 0.4567027371));

	xy.push_back(ofVec2f(0.0398886837, -0.0468833521));
	xy.push_back(ofVec2f(0.0316718407, 0.0516602844));
	xy.push_back(ofVec2f(-0.0379085615, 0.0206585843));
	xy.push_back(ofVec2f(-0.0151045956, 0.0077264998));
	xy.push_back(ofVec2f(0.0238197707, 0.0325947478));
	xy.push_back(ofVec2f(0.0141222430, 0.0217483565));
	xy.push_back(ofVec2f(0.0002901061, 0.0182518009));
	xy.push_back(ofVec2f(0.0466207266, 0.0338115618));
	xy.push_back(ofVec2f(-0.0315673873, 0.0459829457));
	xy.push_back(ofVec2f(0.0138158584, -0.0260905977));
	
	for (int i=0; i<xy.size(); i++) {
		xy[i].x = (xy[i].x + 1.0f) / 2.0f * 1024;
		xy[i].y = (1.0f - xy[i].y) / 2.0f * 768;
	}
	
	vector<Mat> boardRotations, boardTranslations;
	
	vector<vector<Point3f> > vvo(1); //object points
	vector<vector<Point2f> > vvi(1); //image points
	
	
	int flags = 0;
	
	flags |= CV_CALIB_USE_INTRINSIC_GUESS;
	//flags |= CV_CALIB_FIX_PRINCIPAL_POINT;
	flags |= CV_CALIB_FIX_ASPECT_RATIO;
	flags |= CV_CALIB_ZERO_TANGENT_DIST;
	flags |= CV_CALIB_FIX_K1;
	flags |= CV_CALIB_FIX_K2;
	flags |= CV_CALIB_FIX_K3;
	
	
	//pull values into the 3d calibration
	vvo[0] = *(vector<Point3f >*)&xyz;
	vvi[0] = *(vector<Point2f >*)&xy;
	
	//initialised throw ratio = 1.0f
	float f = 1024;
	cameraMatrix = (Mat1d(3, 3) <<
					f, 0, f/2,
					0, f, f/2,
					0, 0, 1);
	try {
		error = calibrateCamera(vvo, vvi, cv::Size(1024, 768), cameraMatrix, distCoeffs, boardRotations, boardTranslations, flags);
	} catch (...) {
		ofLogError() << "Calibration threw exception. Check parameters.";
	}
	
	intrinsics.setup(cameraMatrix, cv::Size(1024, 768));
	
	cout << cameraMatrix << endl;
	cout << distCoeffs << endl;
	cout << "Error: " << error << endl;

}

//--------------------------------------------------------------
void testApp::update(){
	}

//--------------------------------------------------------------
void testApp::draw(){
	ofVec2f screen;
	for (int i=0; i<xy.size(); i++) {
		screen = project(xyz[i]);
		
		ofCircle(screen.x, screen.y, 10);
	}
}

ofVec2f testApp::project(ofVec3f v) {
	vector<cv::Point3f> vo;
	vo.push_back(toCv(v));
	Mat mt = (Mat1d(3,1) << 0, 0, 0);
	Mat mr = (Mat1d(3,1) << 0, 0, 0);
	
	vector<Point2f> projected(1);
	
	projectPoints(Mat(vo), mr, mt, cameraMatrix, distCoeffs, projected);
	
	return toOf(projected[0]);
}
