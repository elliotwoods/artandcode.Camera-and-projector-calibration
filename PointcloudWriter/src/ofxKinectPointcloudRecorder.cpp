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
    folderCount = 0;
    
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

void ofxKinectPointcloudRecorder::incrementFolder(ofImage posterFrame){
    currentFolderPrefix = "TAKE_" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" + ofToString(ofGetSeconds());
    ofDirectory dir(targetDirectory + "/" + currentFolderPrefix);
    
	if(!dir.exists()){
		dir.create(true);
	}
    try{
        posterFrame.saveImage(targetDirectory+"/"+currentFolderPrefix+"/_poster.png");
    }catch(...){
        
    }
    currentFrame = 0;
}

//void ofxKinectPointcloudRecorder::collectIntrinsics(ofxKinect& kinect) {
//	
//
//}

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
			char filenumber[512];
            sprintf(filenumber, "%05d", currentFrame); 
            
			string filename = targetDirectory +  "/" + currentFolderPrefix + "/" + targetFilePrefix + "_" + filenumber +  ".xkcd";
			ofFile file(filename, ofFile::WriteOnly, true);
			file.write( (char*)&tosave[0], sizeof(unsigned short)*640*480 );					   
			file.close();
						
			currentFrame++;
			delete tosave;			
		}
	}
}

unsigned short* ofxKinectPointcloudRecorder::readDepthFrame(string filename) {
	int amnt;
	ofFile infile(filename, ofFile::ReadOnly, true);
	return readDepthFrame(infile);
}

unsigned short* ofxKinectPointcloudRecorder::readDepthFrame(ofFile infile){
	unsigned short* outbuf = new unsigned short[640*480];
	infile.read((char*)(&outbuf[0]), sizeof(unsigned short)*640*480);
	infile.close();
	return outbuf;
}
