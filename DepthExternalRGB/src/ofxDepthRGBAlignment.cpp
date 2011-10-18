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

void ofxDepthRGBAlignment::setup() {
	hasDepthImage = false;
	hasColorImage = false;

}

//-----------------------------------------------
void ofxDepthRGBAlignment::addCalibrationImagePair(ofPixels &depth, ofPixels &color) {
	//TODO:
}

//-----------------------------------------------

void ofxDepthRGBAlignment::setCalibrationDirectoryPair(string depthImageDirectory, string colorImageDirectory){
	
	depthCalibration.calibrateFromDirectory(depthImageDirectory);
	colorCalibration.calibrateFromDirectory(colorImageDirectory);
	
	depthCalibration.getTransformation(colorCalibration, rotationDepthToColor, translationDepthToColor);
	colorCalibration.getTransformation(depthCalibration, rotationColorToDepth, translationColorToDepth);
	
	cout << "Kinect to Color:" << endl << rotationDepthToColor << endl << translationDepthToColor << endl;
	cout << "Color to Kinect:" << endl << rotationColorToDepth << endl << translationColorToDepth << endl;
	
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
	if(hasColorImage && hasDepthImage){
		updatePointCloud();
		updateColors();
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
	//	int w = curKinect.getWidth();
	//	int h = curKinect.getHeight();
	//	float* pixels = curKinect.getPixels();
	
	int i = 0;
	
	/*
	 principalPoint.x += ofMap(mouseX, 0, ofGetWidth(), -4, 4);
	 principalPoint.y += ofMap(mouseY, 0, ofGetHeight(), -4, 4);
	 cout << "fudge: " << ofMap(mouseX, 0, ofGetWidth(), -4, 4) << "x" << ofMap(mouseY, 0, ofGetHeight(), -4, 4) << endl;
	 */
	
	for(int y = 0; y < h; y++) {
		for(int j = 0; j < w; j++) {
			//float pixel = curKinect.at<float>(y, j);
			//float pixel = currentDepthImage.getColor(j, y).getBrightness(); //replace with saved format
			float pixel = currentDepthImage[i];
			if(pixel < 1000) { // the rest is basically noise
				int x = Xres - j - 1; // x axis is flipped from depth image
				float z = rawToCentimeters(pixel);
				
				float xReal = (((float) x - principalPoint.x) / imageSize.width) * z * fx;
				float yReal = (((float) y - principalPoint.y) / imageSize.height) * z * fy;
				
				// add each point into pointCloud
				pointCloud.push_back(Point3f(xReal, yReal, z));
			}									
			i++;
		}
	}	
}

void ofxDepthRGBAlignment::updateColors() {
	imagePoints.clear();
	
	// rotate, translate the points to fit the colorCalibration perspective
	// and project them onto the colorCalibration image space
	// and undistort them
	projectPoints(Mat(pointCloud),
				  rotationDepthToColor, translationDepthToColor,
				  colorCalibration.getDistortedIntrinsics().getCameraMatrix(),
				  colorCalibration.getDistCoeffs(),
				  imagePoints);
	
	// get the color at each of the projectedPoints inside curColor
	// add them into pointCloudColors
	pointCloudColors.clear();
	int w = currentColorImage.getWidth();
	int h = currentColorImage.getHeight();
	int n = w * h;
	unsigned char* pixels = currentColorImage.getPixels();
	for(int i = 0; i < imagePoints.size(); i++) {
		int j = (int) imagePoints[i].y * w + (int) imagePoints[i].x;
		if(j < 0 || j >= n) {
			pointCloudColors.push_back(Point3f(1, 1, 1));
		} else {
			j *= 3;
			pointCloudColors.push_back(Point3f(pixels[j + 0] / 255.f, pixels[j + 1] / 255.f, pixels[j + 2] / 255.f));
		}
	}
}
	
void ofxDepthRGBAlignment::drawMesh() {

}

void ofxDepthRGBAlignment::drawPointCloud() {
	
	ofSetColor(255);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Point3f), &(pointCloudColors[0].x));
	glVertexPointer(3, GL_FLOAT, sizeof(Point3f), &(pointCloud[0].x));
	glDrawArrays(GL_POINTS, 0, pointCloud.size());
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_DEPTH_TEST);
	
	ofSetColor(255);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(Point2f), &(imagePoints[0].x));
	glDrawArrays(GL_POINTS, 0, pointCloud.size());
	glDisableClientState(GL_VERTEX_ARRAY);	
}

