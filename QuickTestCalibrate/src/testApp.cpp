#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    loaded = false;
    calibrated = false;
    videoLoaded = false;
    
    currentCloud = new unsigned short[640*480];
    cam.speed = 5;
    cam.autosavePosition = true;
    cam.useArrowKeys = false;
    cam.loadCameraPosition();
    cam.setScale(1, -1, 1);
    ofBackground(75);
    
    //testImage.loadImage("MVI_9301.MOV.png");
    alignment.setup(10, 7, 4);
    alignment.setColorImage(testImage);

    externalCheckers.setup(10, 7, 4);
    kinectCheckers.setup(10, 7, 4);
    
    scrubVideo = false;
    frameASet = false;
    frameBSet = false;
    
    hideCalibrationDebug = false;
    scrubbing = false;
    
    currentDepthFrame = 0;
    currentVideoFrame = 0;
    
    currentImage = 0;
    calibrationLoaded = false;
    playing = false;
    
    settingsSaveFile = "settings.xml";
    if(recentSaves.loadFile(settingsSaveFile)){
        cout << "preloading video " << 
        loadVideoFile(recentSaves.getValue("videoFile", ""));
        pointClouds.allowExt("xkcd");
        pointClouds.listDir(recentSaves.getValue("cloudFolder", ""));
        recentSaves.saveFile(settingsSaveFile);
        
        cout << "listed " << pointClouds.numFiles() << endl;
        loaded = true;
        
    }
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
    pixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void testApp::update(){
//    image.loadImage("calibration/external/MVI_9292.MOV.png");
    //image.loadImage("testimage.jpg");
    
    if(calibrationLoaded){
        kinectCheckers.setTestImage(kinectImages[currentImage].getPixelsRef());
        externalCheckers.setTestImage(externalImages[currentImage].getPixelsRef());
    }
    
    calibrated = alignment.ready();
    if(loaded && calibrated && videoLoaded){
        bool videoFrameUpdated = false;
        bool depthFrameUpdated = false;
        if(scrubbing){
            if(frameASet && frameBSet){
                float scrubPercent = ofMap(mouseX, 0, ofGetWidth(), 0, 1.0);    
                currentVideoFrame = ofMap(scrubPercent, 0, 1, videoFrameA, videoFrameB);
                currentDepthFrame = ofMap(scrubPercent, 0, 1, depthFrameA, depthFrameB);
                videoFrameUpdated = depthFrameUpdated = true;
            }
            else if(scrubVideo) {
                currentVideoFrame = ofMap(mouseX, 0, ofGetWidth(), 0, video.getTotalNumFrames()-1, true);
                videoFrameUpdated = true;
            }
            else {
                currentDepthFrame = ofMap(mouseX, 0, ofGetWidth(), 0, pointClouds.numFiles()-1, true);
                depthFrameUpdated = true;
            }   
        }
        if(playing){ 
            currentDepthFrame++; 
            currentVideoFrame++;
            videoFrameUpdated = depthFrameUpdated = true;
        } 

        if(videoFrameUpdated){
            video.setFrame(currentVideoFrame);
            video.update();
            testImage.setFromPixels(video.getPixelsRef());            
            alignment.setColorImage(testImage);
        }
            
        if(depthFrameUpdated){
            decoder.readDepthFrame(pointClouds.getPath( currentDepthFrame ), currentCloud );
            //cout << "current depth frame is " << currentDepthFrame << endl;
        }
        
        alignment.updatePointCloud(currentCloud, 640, 480);
    }		
}

//--------------------------------------------------------------
void testApp::draw(){
    
    fbo.begin();{
        ofClear(75,75,75);
        
        if(loaded && calibrated && videoLoaded){
            ofVec3f spinVec(1,0,0);
            //spinVec.rotate(ofGetMouseY()/2.0, ofVec3f(0,1,0));
            meshViewer.setPosition( spinVec * ofGetMouseX() );
            cout << " center! " << alignment.getMeshCenter() << endl;
                                   meshViewer.lookAt(alignment.getMeshCenter());
            meshViewer.lookAt(ofVec3f(0,0,0));
            cout << "cam pos " << cam.getPosition() << endl;
        }
        
        fbo.begin();
        ofClear(75, 75, 75);
        
        meshViewer.begin();
        if(loaded && calibrated && videoLoaded){
            //alignment.drawPointCloud();
            alignment.drawMesh();
        }
        
        if(calibrated && !loaded){
            alignment.drawCalibration(mouseX > ofGetWidth()/2);
        }
        meshViewer.end();    
    } fbo.end();
    
    fbo.getTextureReference().draw(0,0);    

    if(!hideCalibrationDebug && calibrationLoaded){
        externalImages[currentImage].draw(ofRectangle(0,0,320,240));
        kinectImages[currentImage].draw(ofRectangle(320,0,853,480));        
        kinectCheckers.draw(ofRectangle(320,0,853, 480));
        externalCheckers.draw(ofRectangle(0,0,320,240));
    }
    ofSetColor(255, 255, 255);
    
    string debugString = string("scrubbing? ") + (scrubbing ? "YES" : "NO") + "\n";
    debugString += string("scrubbing video? ") + (scrubVideo ? "YES" : "NO") + "\n";
    debugString += string("current video frame ") + ofToString(currentVideoFrame) + "\n";
    debugString += string("current depth frame ") + ofToString(currentDepthFrame) + "\n";
    ofDrawBitmapString(debugString, ofPoint(30,30));
    
    if(playing){
        ofDirectory dir("outframes/" + xmlSaveFile);
        if(!dir.exists()){
            dir.create(true);
        }
        
        fbo.getTextureReference().readToPixels(pixels);
        char pixname[1024];
        sprintf(pixname, "outframes/%s/FRAME_%05d.png",xmlSaveFile.c_str(), ofGetFrameNum());
        ofSaveImage(pixels, pixname);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
    if(key == 'C'){
        /*
        //alignment.calibrateFromDirectoryPair("calibration/kinect","calibration/external");
        ofDirectory kinect, external;
        kinect.listDir("calibration/kinect");
        external.listDir("calibration/external");
        for(int i = 0; i < kinect.numFiles(); i++){
            ofImage image;
            image.loadImage(kinect.getPath(i));
            image.setImageType(OF_IMAGE_GRAYSCALE);
            kinectImages.push_back(image);  
        }
        for(int i = 0; i < external.numFiles(); i++){
            ofImage image;
            image.loadImage(external.getPath(i));
            image.setImageType(OF_IMAGE_GRAYSCALE);
            externalImages.push_back(image);  
        }
        currentImage = 0;
        cout << "loaded " << kinectImages.size() << " " << externalImages.size() << endl;
        */
        
        alignment.calibrateFromDirectoryPair("calibration/kinect","calibration/external");
        calibrated = true;
        /*
        ofFileDialogResult r = ofSystemLoadDialog("Open ColorCamera");
        if(r.bSuccess){
            string directory1 = r.getPath();
            r = ofSystemLoadDialog();
            if(r.bSuccess){
                alignment.calibrateFromDirectoryPair(directory1, r.getPath());
            }
        }
        */
        
        //calibrationLoaded = true;
    }
    
    if(key == 'l'){
        ofFileDialogResult r = ofSystemLoadDialog("Load Kinect Cloud", true);
        if(r.bSuccess){
            pointClouds.allowExt("xkcd");
            pointClouds.listDir(r.getPath());
            recentSaves.setValue("cloudFolder", r.getPath());
            recentSaves.saveFile(settingsSaveFile);
            
            cout << "listed " << pointClouds.numFiles() << endl;
            loaded = true;
            
        }
    }
    
    if(key == 'v'){
        ofFileDialogResult r = ofSystemLoadDialog("Load Video Data", false);
        if(r.bSuccess){
            loadVideoFile(r.getPath());
        }
    }
    
    if(videoLoaded && loaded && calibrated){
        if(key == '1'){
            depthFrameA = currentDepthFrame;
            videoFrameA = currentVideoFrame;
            frameASet = true;
            videosave.setValue("depthFrameA", depthFrameA);
            videosave.setValue("videoFrameA", videoFrameA);
            videosave.saveFile(xmlSaveFile);
            cout << "saving file " << xmlSaveFile << endl;
        }
        
        if(key == '2'){
            depthFrameB = currentDepthFrame;
            videoFrameB = currentVideoFrame;
            frameBSet = true;
            videosave.setValue("depthFrameB", depthFrameB);
            videosave.setValue("videoFrameB", videoFrameB);
            videosave.saveFile(xmlSaveFile);
            cout << "saving file " << xmlSaveFile << endl;
        }
    }
    
    if(key == ' '){
        scrubVideo = !scrubVideo;
    }
    
    if(key == 'r'){
        frameBSet = frameASet = false;
    }
    
    if(calibrationLoaded){
        if(key == OF_KEY_LEFT){
            currentImage = (currentImage + 1) % kinectImages.size();
            cout << "showing image " << currentImage << endl;
        }
        else if(key == OF_KEY_RIGHT){
            currentImage = (kinectImages.size() + currentImage - 1) % kinectImages.size();            
            cout << "showing image " << currentImage << endl;
        }
        
        if(key == '+'){
            alignment.addCalibrationImagePair(kinectImages[currentImage].getPixelsRef(), 
                                              externalImages[currentImage].getPixelsRef());
            cout << "added images " << endl;
        }
    }
    

    if(key == 'h'){
        hideCalibrationDebug = !hideCalibrationDebug;
    }
    
    if(key == 'p'){
        playing = !playing;
    }
    
    if(key == '/'){
        scrubbing = !scrubbing;
    }

}
void testApp::loadVideoFile(string filePath){
    videoLoaded = video.loadMovie(filePath);
    xmlSaveFile = ofFilePath::getBaseName(filePath) + ".xml";
    if(videoLoaded){
        recentSaves.setValue("videoFile", filePath);
        recentSaves.saveFile(settingsSaveFile);
        if(videosave.loadFile(xmlSaveFile)){
            depthFrameA = videosave.getValue("depthFrameA", 0);
            depthFrameB = videosave.getValue("depthFrameB", 0);
            videoFrameA = videosave.getValue("videoFrameA", 0);
            videoFrameB = videosave.getValue("videoFrameB", 0);
            frameASet = true;
            frameBSet = true;
        }
    }
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
    fbo.allocate(w, h, GL_RGB, 4);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}