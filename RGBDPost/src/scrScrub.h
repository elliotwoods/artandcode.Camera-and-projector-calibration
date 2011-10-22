
#pragma once

#include "ofMain.h"
#include "ofxCVgui.h"

class scrScrub : public scrBase {
  public:
	scrScrub(string caption);
	
    float getCurrentSelectedValue();
    float getCurrentScrubValue();
    
    void mousePressed(float x, float y, int button);
	void mouseDragged(float x, float y, float dx, float dy, int button);
	
	void keyPressed(int key);

  protected:
    void drawContent();

    float currentScrubValue;
    float currentSelectValue;
};