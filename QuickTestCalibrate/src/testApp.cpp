#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    loaded = false;
    calibrated = false;
    videoLoaded = false;
    
    currentCloud = new unsigned short[640*480];
    cam.speed = 5;
    cam.autosavePosition = true;
    cam.loadCameraPosition();
    
    //testImage.loadImage("MVI_9301.MOV.png");
    alignment.setColorImage(testImage);

}

//--------------------------------------------------------------
void testApp::update(){
//    image.loadImage("calibration/external/MVI_9292.MOV.png");
    //image.loadImage("testimage.jpg");
    if(loaded && calibrated && videoLoaded){
        int index = ofMap(mouseX, 0, ofGetWidth(), 0, pointClouds.numFiles()-1, true);
        decoder.readDepthFrame(pointClouds.getPath( index ), currentCloud );
        testImage.setFromPixels(video.getPixelsRef());
        alignment.setColorImage(testImage);
        alignment.updatePointCloud(currentCloud, 640, 480);
    }
    
    if(videoLoaded){
        video.update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    cam.begin();
    if(loaded && calibrated && videoLoaded){
        //alignment.drawPointCloud();
        alignment.drawMesh();
    }
    
    if(calibrated && !loaded){
        alignment.drawCalibration(mouseX > ofGetWidth()/2);
    }
    cam.end();
   // image.draw(0,0);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'C'){
        alignment.calibrateFromDirectoryPair("calibration/kinect","calibration/external");
        
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
        
        calibrated = true;
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
            video.play();
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}