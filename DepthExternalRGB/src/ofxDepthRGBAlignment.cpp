/*
 *  ofxDepthRGBAlignment.cpp
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/18/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#include "ofxDepthRGBAlignment.h"

const float
k1 = 0.1236,
k2 = 2842.5,
k3 = 1.1863,
k4 = 0.0370;

static float rawToCentimeters(float raw) {
	return 100 * (k1 * tan((raw / k2) + k3) - k4);
}

ofxDepthRGBAlignment::ofxDepthRGBAlignment() {
	
}

//-----------------------------------------------

ofxDepthRGBAlignment::~ofxDepthRGBAlignment() {
	
}

//-----------------------------------------------

void ofxDepthRGBAlignment::setup(int squaresWide, int squaresTall, int squareSize) {
	hasDepthImage = false;
	hasColorImage = false;
	
	depthCalibration.setPatternSize(squaresWide, squaresTall);
	depthCalibration.setSquareSize(squareSize);
	
	colorCalibration.setPatternSize(squaresWide, squaresTall);
	colorCalibration.setSquareSize(squareSize);

}

//-----------------------------------------------
bool ofxDepthRGBAlignment::addCalibrationImagePair(ofPixels &ir, ofPixels &camera) {

	if(depthCalibration.add(toCv(ir))){
		if(!colorCalibration.add(toCv(camera))){
			depthCalibration.imagePoints.erase(depthCalibration.imagePoints.end()-1);
		}		
	}
	
	if(depthCalibration.imagePoints.size() != colorCalibration.imagePoints.size()){
		ofLogError("ofxDepthRGBAlignment -- image point sizes differ!");
		return false;
	}
	
	if(depthCalibration.imagePoints.size() > 3){
		depthCalibration.calibrate();
		colorCalibration.calibrate();
	}
	
	if(depthCalibration.isReady() && colorCalibration.isReady()){
		depthCalibration.getTransformation(colorCalibration, rotationDepthToColor, translationDepthToColor);
		colorCalibration.getTransformation(depthCalibration, rotationColorToDepth, translationColorToDepth);
		
		cout << "Kinect to Color:" << endl << rotationDepthToColor << endl << translationDepthToColor << endl;
		cout << "Color to Kinect:" << endl << rotationColorToDepth << endl << translationColorToDepth << endl;

		return true;
	}
	
	//cout << " NOT READY! " << depthCalibration.isReady() << " " <<  colorCalibration.isReady()  << " ? ?" << depthCalibration.imagePoints.size() << " " << colorCalibration.imagePoints.size() << endl;
	return false;
}

//-----------------------------------------------

bool ofxDepthRGBAlignment::setCalibrationDirectoryPair(string depthImageDirectory, string colorImageDirectory){
	
	depthCalibration.calibrateFromDirectory(depthImageDirectory);
	colorCalibration.calibrateFromDirectory(colorImageDirectory);
	
	depthCalibration.getTransformation(colorCalibration, rotationDepthToColor, translationDepthToColor);
	colorCalibration.getTransformation(depthCalibration, rotationColorToDepth, translationColorToDepth);
	
	cout << "Kinect to Color:" << endl << rotationDepthToColor << endl << translationDepthToColor << endl;
	cout << "Color to Kinect:" << endl << rotationColorToDepth << endl << translationColorToDepth << endl;
	
	return depthCalibration.isReady() && colorCalibration.isReady();
}

bool ofxDepthRGBAlignment::ready(){
	return depthCalibration.isReady() && colorCalibration.isReady();
}

void ofxDepthRGBAlignment::saveCalibration() {
	depthCalibration.save("depthCalib.yml");	
	colorCalibration.save("colorCalib.yml");
}
void ofxDepthRGBAlignment::loadCalibration() {
	depthCalibration.load("depthCalib.yml");
	colorCalibration.load("colorCalib.yml");
//	depthCalibration.calibrate();
//	colorCalibration.calibrate();
	
	depthCalibration.getTransformation(colorCalibration, rotationDepthToColor, translationDepthToColor);
	colorCalibration.getTransformation(depthCalibration, rotationColorToDepth, translationColorToDepth);
	cout << "Kinect to Color:" << endl << rotationDepthToColor << endl << translationDepthToColor << endl;
	cout << "Color to Kinect:" << endl << rotationColorToDepth << endl << translationColorToDepth << endl;

}

void ofxDepthRGBAlignment::resetCalibration() {
	//TODO:
}
	
/*
void ofxDepthRGBAlignment::saveCalibration(Calibration& from, Calibration& to, string filename) {
	Mat rotation, translation;
	
	from.getTransformation(to, rotation, translation);
	FileStorage fs(ofToDataPath(filename), FileStorage::WRITE);
	
	fs << "rotation" << rotation;
	fs << "translation" << translation;
	
	cout << "rotation:" << endl << rotation << endl;
	cout << "translation:" << endl << translation << endl;
}
*/

//-----------------------------------------------

void ofxDepthRGBAlignment::setColorImage(ofPixels& colorImage) {
	currentColorImage = colorImage;
	hasColorImage = true;
}

//-----------------------------------------------

void ofxDepthRGBAlignment::setDepthImage(unsigned short* depthImage) {
	currentDepthImage = depthImage;
	hasDepthImage = true;
}

void ofxDepthRGBAlignment::update() {

	if(colorCalibration.isReady() && depthCalibration.isReady()){
		updatePointCloud();
		updateColors();
	}
	else {
		//cout << "unready update" << endl;
	}
		
}

void ofxDepthRGBAlignment::updatePointCloud() {
	pointCloud.clear();
	
	const unsigned int Xres = 640;
	const unsigned int Yres = 480;
	
	Point2d fov = depthCalibration.getUndistortedIntrinsics().getFov();
	float fx = tanf(ofDegToRad(fov.x) / 2) * 2;
	float fy = tanf(ofDegToRad(fov.y) / 2) * 2;
	
	Point2d principalPoint = depthCalibration.getUndistortedIntrinsics().getPrincipalPoint();
	cv::Size imageSize = depthCalibration.getUndistortedIntrinsics().getImageSize();
	
	int w = Xres;
	int h = Yres;
	
	//cout << " calculated image size " << imageSize.width << " x " << imageSize.height << endl;
	
	//	int w = curKinect.getWidth();
	//	int h = curKinect.getHeight();
	//	float* pixels = curKinect.getPixels();
	
	int validPointCount = 0;
	
	/*
	 principalPoint.x += ofMap(mouseX, 0, ofGetWidth(), -4, 4);
	 principalPoint.y += ofMap(mouseY, 0, ofGetHeight(), -4, 4);
	 cout << "fudge: " << ofMap(mouseX, 0, ofGetWidth(), -4, 4) << "x" << ofMap(mouseY, 0, ofGetHeight(), -4, 4) << endl;
	 */
	ofVec3f center(0,0,0);
	
	for(int y = 0; y < h; y++) {
		for(int j = 0; j < w; j++) {
			//float pixel = curKinect.at<float>(y, j);
			//float pixel = currentDepthImage.getColor(j, y).getBrightness(); //replace with saved format
			float pixel = currentDepthImage[y*w+j];
			//if(pixel < 1000) { // the rest is basically noise
			//int x = Xres - j - 1; // x axis is flipped from depth image
			int x = j;
			float z;
			if(pixel < 1000){ // the rest is basically noise
				z = rawToCentimeters(pixel);
			}
			else{
				z = 0;
			}	
			
			float xReal = (((float) x - principalPoint.x) / imageSize.width) * z * fx;
			float yReal = (((float) y - principalPoint.y) / imageSize.height) * z * fy;
			
			// add each point into pointCloud
			pointCloud.push_back(Point3f(xReal, yReal, z));
			if(z != 0){				
				center += ofVec3f(xReal, yReal, z);
				validPointCount++;
			}
		}
	}
	
	//cout << " i ended up at " << i << endl;
	
	meshCenter = center / validPointCount;

	meshDistance = 0;
	for(int i = 0; i < pointCloud.size(); i++){
		float thisDistance = center.distance(ofVec3f(pointCloud[i].x,
													 pointCloud[i].y,
													 pointCloud[i].z));
		if(thisDistance > meshDistance){
			meshDistance = thisDistance;
		}
	}
//	cout << "mesh center " << meshCenter << " distance " << meshDistance << endl; 
}

void ofxDepthRGBAlignment::updateColors() {
	pointCloudColors.clear();
	imagePoints.clear();

	// rotate, translate the points to fit the colorCalibration perspective
	// and project them onto the colorCalibration image space
	// and undistort them
	Mat pcMat = Mat(pointCloud);
	
	//cout << "PC " << pcMat << endl;
//	cout << "Rot Depth->Color " << rotationDepthToColor << endl;
//	cout << "Trans Depth->Color " << translationDepthToColor << endl;
//	cout << "Intrs Cam " << colorCalibration.getDistortedIntrinsics().getCameraMatrix() << endl;
//	cout << "Intrs Dist Coef " << colorCalibration.getDistCoeffs() << endl;
	
	projectPoints(pcMat,
				  rotationDepthToColor, translationDepthToColor,
				  colorCalibration.getDistortedIntrinsics().getCameraMatrix(),
				  colorCalibration.getDistCoeffs(),
				  imagePoints);
	
	// get the color at each of the projectedPoints inside curColor
	// add them into pointCloudColors
	int w = currentColorImage.getWidth();
	int h = currentColorImage.getHeight();
	int n = w * h;
	unsigned char* pixels = currentColorImage.getPixels();
	for(int i = 0; i < imagePoints.size(); i++) {
		int j = (int) imagePoints[i].y * w + (int) imagePoints[i].x;
		//pointCloudColors.push_back(Point3f(1, 0, 1));
		if(j < 0 || j >= n) {
			pointCloudColors.push_back(Point3f(1, 0, 0));
		} else {
			j *= 3;
			pointCloudColors.push_back(Point3f(pixels[j + 0] / 255.f, pixels[j + 1] / 255.f, pixels[j + 2] / 255.f));
		}
	}
}


ofVec3f ofxDepthRGBAlignment::getMeshCenter(){
	return meshCenter;
}

float ofxDepthRGBAlignment::getMeshDistance(){
	return meshDistance;
}

void ofxDepthRGBAlignment::drawMesh() {

}

void ofxDepthRGBAlignment::drawPointCloud() {
	
	ofPushStyle();
	
	ofSetColor(255);
	
	glPushMatrix();
	glScaled(1, -1, 1);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Point3f), &(pointCloudColors[0].x));
	glVertexPointer(3, GL_FLOAT, sizeof(Point3f), &(pointCloud[0].x));
	glDrawArrays(GL_POINTS, 0, pointCloud.size());
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_DEPTH_TEST);
	
//	ofSetColor(255);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_FLOAT, sizeof(Point2f), &(imagePoints[0].x));
//	glDrawArrays(GL_POINTS, 0, pointCloud.size());
//	glDisableClientState(GL_VERTEX_ARRAY);	
	
//	if(depthCalibration.isReady()){
//		depthCalibration.draw3d();
//	}
//	if(colorCalibration.isReady()){
//		colorCalibration.draw3d();
//	}
	glPopMatrix();
	
	ofPopStyle();
}

