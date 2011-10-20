/*
 *  ofxKinectPointcloudRecorder.cpp
 *  PointcloudWriter
 *
 *  Created by Jim on 10/20/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#include "ofxKinectPointcloudRecorder.h"

ofxKinectPointcloudRecorder::ofxKinectPointcloudRecorder(){
	
}

void ofxKinectPointcloudRecorder::setup(){
	
	startThread(true, false);
	currentFrame = 0;
}

void ofxKinectPointcloudRecorder::setRecordLocation(string directory, string filePrefix){
	targetDirectory = directory;
	ofDirectory dir(directory);
	if(!dir.exists()){
		dir.create(true);
	}
	
	targetFilePrefix = filePrefix;
}

void ofxKinectPointcloudRecorder::addImage(unsigned short* image){
	unsigned short* addToQueue = new unsigned short[640*480];
	memcpy(addToQueue, image, 640*480);
	lock();
	saveQueue.push( addToQueue );
	unlock();
}

void ofxKinectPointcloudRecorder::threadedFunction(){

	while(isThreadRunning()){
		unsigned short* tosave = NULL;
		lock();
		if(saveQueue.size() != 0){
			tosave = saveQueue.front();
			saveQueue.pop();
			cout << " currently " << saveQueue.size() << " waiting " << endl;
		}
		unlock();
		
		if(tosave != NULL){
			vector<ofVec3f> newpoints;
			for(int i = 0; i < kinect->getHeight()*kinect->getWidth(); i++){
				int distance = tosave[i];
				if(distance > 0){
					ofVec3f worldp = kinect->getWorldCoordinateAt(i%int(kinect->getWidth()), i/int(kinect->getWidth()), distance);
					newpoints.push_back(worldp);
				}
			}
			
			
			string filename = targetDirectory + "/" + targetFilePrefix + "_" + ofToString(currentFrame) +  ".pts";
			ofFile file(filename, ofFile::WriteOnly, true);
			int size = newpoints.size();
			
			file.write( (char*)&size, sizeof(int));
			file.write( (char*)&newpoints[0], sizeof(ofVec3f)*size);					   
			file.close();
			
			/*
			int amnt;
			ofFile openTest(filename, ofFile::ReadOnly, true);
			openTest.read((char*)&amnt,sizeof(int));
			cout << " amount " << amnt << endl;
			
			vector<ofVec3f> testvec(amnt);
			openTest.read((char*)&(testvec[0]), sizeof(ofVec3f)*amnt);
			openTest.close();
			 */
			
			currentFrame++;
			delete tosave;			
		}
	}
}

vector<ofVec3f> ofxKinectPointcloudRecorder::readPointcloud(string filename) {
	int amnt;
	ofFile infile(filename, ofFile::ReadOnly, true);
	return readPointcloud(infile);
}

vector<ofVec3f> ofxKinectPointcloudRecorder::readPointcloud(ofFile infile){
	int amnt;
	infile.read((char*)&amnt,sizeof(int));
	vector<ofVec3f> returnvec(amnt);
	
	infile.read((char*)&(returnvec[0]), sizeof(ofVec3f)*amnt);
	infile.close();
	return returnvec;
}
