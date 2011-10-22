#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    loaded = false;
    calibrated = false;
    videoLoaded = false;
    
    currentCloud = new unsigned short[640*480];
    cam.speed = 2;
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
    
    currentDepthFrame = 0;
    currentVideoFrame = 0;
    
    currentImage = 0;
    calibrationLoaded = false;
    playing = false;
    
//    ofToggleFullscreen();
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
    savePixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
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
        if(frameASet && frameBSet){
            if(!playing){
                float scrubPercent = ofMap(mouseX, 0, ofGetWidth(), 0, 1.0);    
                currentVideoFrame = ofMap(scrubPercent, 0, 1, videoFrameA, videoFrameB);
                currentDepthFrame = ofMap(scrubPercent, 0, 1, depthFrameA, depthFrameB);
            }
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
        
        if(videoFrameUpdated){
            if(playing){
                video.setFrame(currentVideoFrame);
                video.update();
                testImage.setFromPixels(video.getPixelsRef());            
                alignment.setColorImage(testImage);
            }
            //cout << "current video frame is " << currentVideoFrame << endl;
        }
        
        if(depthFrameUpdated){
            decoder.readDepthFrame(pointClouds.getPath( currentDepthFrame ), currentCloud );
            //cout << "current depth frame is " << currentDepthFrame << endl;
        }

        alignment.updatePointCloud(currentCloud, 640, 480);
        if(playing){ 
            currentDepthFrame++; 
            currentVideoFrame++;
        } 
    }
    
//    if(videoLoaded){
//        video.update();
//    }
}

//--------------------------------------------------------------
void testApp::draw(){
    fbo.begin();
    ofClear(75,75,75);
    
    if(loaded && calibrated && videoLoaded){
        meshViewer.setPosition(alignment.getMeshCenter() + ofVec3f(0, 0, 10));
        cout << " center! " << alignment.getMeshCenter() << endl;
                               meshViewer.lookAt(alignment.getMeshCenter());
        meshViewer.lookAt(alignment.getMeshCenter(), ofVec3f(0,1,0));
//        cout << "cam pos " << cam.getPosition() << endl;
    }
    
    cam.begin();
    if(loaded && calibrated && videoLoaded){
        //alignment.drawPointCloud();
        alignment.drawMesh();
    }
    
    if(calibrated && !loaded){
        alignment.drawCalibration(mouseX > ofGetWidth()/2);
    }
    cam.end();
    fbo.end();
    fbo.getTextureReference().draw(0,0);
    
   // image.draw(0,0);
    if(calibrationLoaded && !hideCalibrationDebug){
        externalImages[currentImage].draw(ofRectangle(0,0,320,240));
        kinectImages[currentImage].draw(ofRectangle(320,0,853,480));        
        kinectCheckers.draw(ofRectangle(320,0,853, 480));
        externalCheckers.draw(ofRectangle(0,0,320,240));
    }
 
    if(playing){
        fbo.getTextureReference().readToPixels(savePixels);
        char pixname[1024];
        sprintf(pixname, "outframes/FRAME_%05d.png", ofGetFrameNum());
        ofSaveImage(savePixels, pixname);
                      
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'C'){
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
        
        calibrationLoaded = true;
    }
    
    if(key == 'l'){
        ofFileDialogResult r = ofSystemLoadDialog("Load Kinect Cloud", true);
        if(r.bSuccess){
            pointClouds.allowExt("xkcd");
            pointClouds.listDir(r.getPath());
            cout << "listed " << pointClouds.numFiles() << endl;
        }
        loaded = true;
    }
    
    if(key == 'v'){
        ofFileDialogResult r = ofSystemLoadDialog("Load Video Data", false);
        if(r.bSuccess){
            videoLoaded = video.loadMovie(r.getPath());
            xmlSaveFile = r.getName() + ".xml";
            if(videoLoaded){
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
    
    if(key == 'm'){
        
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