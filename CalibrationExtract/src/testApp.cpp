#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    
}

//--------------------------------------------------------------
void testApp::update(){
    player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    player.draw(0,0,1920/2, 1080/2);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'l'){
        ofFileDialogResult r = ofSystemLoadDialog();
        if(r.bSuccess){
            currentFileName = r.getName();
            player.loadMovie(ofToDataPath(r.getPath()));
            player.play();
        }
    }
    
    if(key == 's'){
        ofImage output;
        output.setFromPixels(player.getPixels(), 
                             player.getWidth(), 
                             player.getHeight(), OF_IMAGE_COLOR);
        output.saveImage(currentFileName + ".png");
        cout << "saving image to " << currentFileName << endl;
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