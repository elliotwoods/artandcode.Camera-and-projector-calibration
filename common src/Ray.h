#pragma once
//
//  ray.h
//  intersectRayTest
//
//  Created by Elliot Woods on 23/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofMain.h"

class Ray {
public:
	Ray(); ///< random initialisation
	Ray(float f);
	
	void draw(float f=0.0f);
	
	void randomise();
	
	Ray intersect(Ray &other);
	void transform(ofMatrix4x4 m);
	
	ofVec3f	getMidpoint() const;
	float	getLength() const;
	
	ofVec3f		s, t;
	ofColor		color;
	
	bool	infinite;
};