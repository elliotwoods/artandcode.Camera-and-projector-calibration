/*
 *  ofxRGBDAlignment.cpp
 *  DepthExternalRGB
 *
 *  Created by Jim on 10/18/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#include "ofxRGBDAlignment.h"

const float
k1 = 0.1236,
k2 = 2842.5,
k3 = 1.1863,
k4 = 0.0370;

static float rawToCentimeters(float raw) {
	return 100 * (k1 * tan((raw / k2) + k3) - k4);
}

ofxRGBDAlignment::ofxRGBDAlignment() {
	
}

//-----------------------------------------------

ofxRGBDAlignment::~ofxRGBDAlignment() {
	
}

//-----------------------------------------------

void ofxRGBDAlignment::setup(int squaresWide, int squaresTall, int squareSize) {
	hasDepthImage = false;
	hasColorImage = false;
	
	depthCalibration.setPatternSize(squaresWide, squaresTall);
	depthCalibration.setSquareSize(squareSize);
	
	colorCalibration.setPatternSize(squaresWide, squaresTall);
	colorCalibration.setSquareSize(squareSize);
	
//	mesh.setUsage(GL_STREAM_DRAW);
	int width = 640;
	int height = 480;
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			mesh.addVertex(ofPoint(x,y,0));	// mesh index = x + y*width
			mesh.addTexCoord(ofVec2f(0,0));
			//mesh.addColor(ofFloatColor(0,0,0));
		}
	}
}

//-----------------------------------------------
bool ofxRGBDAlignment::addCalibrationImagePair(ofPixels &ir, ofPixels &camera) {

	if(depthCalibration.add(toCv(ir))){
		if(!colorCalibration.add(toCv(camera))){
			depthCalibration.imagePoints.erase(depthCalibration.imagePoints.end()-1);
		}		
	}
	
	if(depthCalibration.imagePoints.size() != colorCalibration.imagePoints.size()){
		ofLogError("ofxRGBDAlignment -- image point sizes differ!");
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

bool ofxRGBDAlignment::calibrateFromDirectoryPair(string depthImageDirectory, string colorImageDirectory){
	
	depthCalibration.calibrateFromDirectory(depthImageDirectory);
	colorCalibration.calibrateFromDirectory(colorImageDirectory);
	
	depthCalibration.getTransformation(colorCalibration, rotationDepthToColor, translationDepthToColor);
	colorCalibration.getTransformation(depthCalibration, rotationColorToDepth, translationColorToDepth);
	
	cout << "Kinect to Color:" << endl << rotationDepthToColor << endl << translationDepthToColor << endl;
	cout << "Color to Kinect:" << endl << rotationColorToDepth << endl << translationColorToDepth << endl;
	
	return depthCalibration.isReady() && colorCalibration.isReady();
}

bool ofxRGBDAlignment::ready(){
	return depthCalibration.isReady() && colorCalibration.isReady();
}

void ofxRGBDAlignment::saveCalibration() {
	depthCalibration.save("depthCalib.yml");	
	colorCalibration.save("colorCalib.yml");
}

void ofxRGBDAlignment::loadCalibration() {
	depthCalibration.load("depthCalib.yml");
	colorCalibration.load("colorCalib.yml");
	
	depthCalibration.getTransformation(colorCalibration, rotationDepthToColor, translationDepthToColor);
	colorCalibration.getTransformation(depthCalibration, rotationColorToDepth, translationColorToDepth);
	
	cout << "Kinect to Color:" << endl << rotationDepthToColor << endl << translationDepthToColor << endl;
	cout << "Color to Kinect:" << endl << rotationColorToDepth << endl << translationColorToDepth << endl;

}

void ofxRGBDAlignment::resetCalibration() {
	//TODO:
}
	
/*
void ofxRGBDAlignment::saveCalibration(Calibration& from, Calibration& to, string filename) {
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

void ofxRGBDAlignment::setColorImage(ofImage& colorImage) {
	currentColorImage = colorImage;
	hasColorImage = true;
}

//-----------------------------------------------

void ofxRGBDAlignment::setDepthImage(unsigned short* depthImage) {
	currentDepthImage = depthImage;
	hasDepthImage = true;
}

void ofxRGBDAlignment::update() {

	if(colorCalibration.isReady() && depthCalibration.isReady()){
//		updatePointCloud();
//		cout << "PP " << depthCalibration.getUndistortedIntrinsics().getPrincipalPoint() << endl;
//		updateColors();
//		updateMesh();
	}		
}

void ofxRGBDAlignment::updatePointCloud(ofxKinect& kinect){
 	int w = kinect.getWidth();
	int h = kinect.getHeight();
    
    updatePointCloud(kinect.getRawDepthPixels(), w, h);
}

void ofxRGBDAlignment::updatePointCloud(unsigned short* depthPixelsRaw, int w, int h){
	
	////METHOD 1 custom cloud
	
	pointCloud.clear();

	Point2d fov = depthCalibration.getUndistortedIntrinsics().getFov();
	float fx = tanf(ofDegToRad(fov.x) / 2) * 2;
	float fy = tanf(ofDegToRad(fov.y) / 2) * 2;
	
	Point2d principalPoint = depthCalibration.getUndistortedIntrinsics().getPrincipalPoint();
	cv::Size imageSize = depthCalibration.getUndistortedIntrinsics().getImageSize();

    currentDepthImage = depthPixelsRaw;

	int validPointCount = 0;
	ofVec3f center(0,0,0);
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {

			//float pixel = rawToCentimeters( currentDepthImage[y*w+j] );
            unsigned short z = currentDepthImage[y*w+x];
            float xReal = (((float) x - principalPoint.x) / imageSize.width) * z * fx;
            float yReal = (((float) y - principalPoint.y) / imageSize.height) * z * fy;
            // add each point into pointCloud
            pointCloud.push_back(Point3f(xReal, yReal, z));
            if(z > 1){
                //pointCloud.push_back(Point3f(x/640.0, y/640., 0));
                center += ofVec3f(xReal, yReal, z);
                validPointCount++;
            }
		}
	}
    if(ofGetFrameNum() % 100 == 0){
        cout << validPointCount << " vertices added " << endl;
    }
    
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
    
    cout << "mesh center " <<  meshCenter << endl;

		
/*
	////METHOD 2 kinect's cloud
	for(int y = 0; y < kinect.getHeight(); y++){
		for(int x = 0; x < kinect.getWidth(); x++){
			float color = kinect.getPixelsRef().getColor(x, y).getBrightness();
			ofVec3f worldp = kinect.getWorldCoordinateAt(x, y);
			pointCloud.push_back(toCv(worldp));
		}
	}
*/
	

	updateColors();
    updateMesh();
}

void ofxRGBDAlignment::updateMesh() {
	
	Mat pcMat = Mat(pointCloud);
	
	//cout << "PC " << pcMat << endl;
	//	cout << "Rot Depth->Color " << rotationDepthToColor << endl;
	//	cout << "Trans Depth->Color " << translationDepthToColor << endl;
	//	cout << "Intrs Cam " << colorCalibration.getDistortedIntrinsics().getCameraMatrix() << endl;
	//	cout << "Intrs Dist Coef " << colorCalibration.getDistCoeffs() << endl;
	
	imagePoints.clear();
	projectPoints(pcMat,
				  rotationDepthToColor, translationDepthToColor,
				  colorCalibration.getDistortedIntrinsics().getCameraMatrix(),
				  colorCalibration.getDistCoeffs(),
				  imagePoints);
	
	int w = 640;
	int h = 480;
    texcoords.clear();
    vertices.clear();
	for(int i = 0; i < imagePoints.size(); i++) {
		ofVec2f textureCoord = ofVec2f(imagePoints[i].x,imagePoints[i].y);
		int j = (int)imagePoints[i].y * currentColorImage.getWidth() + (int) imagePoints[i].x;
		ofFloatColor color;
		color = ofFloatColor(1, 1, 1, 1);		
        vertices.push_back( toOf(pointCloud[i]) );
        texcoords.push_back( textureCoord );
        
		//mesh.setColor(i, color);
		mesh.setTexCoord(i, textureCoord);
		mesh.setVertex(i, toOf(pointCloud[i]));
	}
    
	int facesAdded = 0;
	mesh.clearIndices();
    indeces.clear();
    zthresh = .3;
	for (int y = 0; y < h-1; y++){
		for (int x=0; x < w-1; x++){
			if(pointCloud[x+y*w].z > zthresh ||
			   pointCloud[(x+1)+y*w].z > zthresh ||
			   pointCloud[x+(y+1)*w].z > zthresh)
			{
                indeces.push_back(x+y*w);
                indeces.push_back((x+1)+y*w);
                indeces.push_back(x+(y+1)*w);
                
				mesh.addIndex(x+y*w);				// 0
				mesh.addIndex((x+1)+y*w);			// 1
				mesh.addIndex(x+(y+1)*w);			// 10				
				facesAdded++;
			}
			
			if(pointCloud[(x+1)+y*w].z > zthresh ||
			   pointCloud[x+(y+1)*w].z > zthresh ||
			   pointCloud[(x+1)+(y+1)*w].z > zthresh)
			{
                indeces.push_back((x+1)+y*w);
                indeces.push_back(x+(y+1)*w);
                indeces.push_back((x+1)+(y+1)*w);
                
				mesh.addIndex((x+1)+y*w);			// 1
				mesh.addIndex(x+(y+1)*w);			// 10
				mesh.addIndex((x+1)+(y+1)*w);		// 11
				facesAdded++;
			}
		}
	}

//    vbo.setIndexData(&indeces[0], indeces.size(), GL_STREAM_DRAW);
//    vbo.setVertexData(&vertices[0], vertices.size(), GL_STREAM_DRAW);
//    vbo.setTexCoordData(&texcoords[0], texcoords.size(), GL_STREAM_DRAW);
    
    if(ofGetFrameNum() % 100 == 0){
        cout << "faces added " << facesAdded << endl;
    }
}

/*
void ofxRGBDAlignment::setPointCloud(vector<Point3f>& newCloud){
//	pointCloud.clear();
//	for(int i = 0; i < newCloud.size(); i++){
//		pointCloud.push_back( toCv(newCloud[i]) );
//	}
	pointCloud = newCloud;
	if(colorCalibration.isReady() && depthCalibration.isReady()){
		updateColors();
		//updateMesh();
	}
}
*/

void ofxRGBDAlignment::updateColors() {
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
		//pointCloudColors.push_back(Point3f(1, 1, 1));
		if(j < 0 || j >= n) {
			pointCloudColors.push_back(Point3f(0, 0, 0));
		} else {
			j *= 3;
			pointCloudColors.push_back(Point3f(pixels[j + 0] / 255.f, pixels[j + 1] / 255.f, pixels[j + 2] / 255.f));
		}
	}
}

void ofxRGBDAlignment::drawCalibration(bool left){
    if(left){
        depthCalibration.draw3d();
    }
    else{
        colorCalibration.draw3d();
    }
}

ofVec3f ofxRGBDAlignment::getMeshCenter(){
	return meshCenter;
}

float ofxRGBDAlignment::getMeshDistance(){
	return meshDistance;
}

void ofxRGBDAlignment::drawMesh() {
	glPushMatrix();
	glScaled(1, -1, 1);

	glEnable(GL_DEPTH_TEST);
	currentColorImage.getTextureReference().bind();
	mesh.drawFaces();
    //vbo.drawElements(GL_TRIANGLES, indeces.size()/3);
	currentColorImage.getTextureReference().unbind();
	glDisable(GL_DEPTH_TEST);
	
	glPopMatrix();


}

void ofxRGBDAlignment::drawPointCloud() {
	
	ofPushStyle();
	
	ofSetColor(255);
	glPointSize(2);
    
	glPushMatrix();
	glScaled(1, -1, 1);
	
	glEnable(GL_DEPTH_TEST);
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

