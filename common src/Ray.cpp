//
//  Ray.cpp
//  intersectRayTest
//
//  Created by Elliot Woods on 23/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "Ray.h"

Ray::Ray() {
}

Ray::Ray(float f) {
	infinite = false;
}

void Ray::draw(float f) {
	ofPushStyle();
	ofEnableSmoothing();
	ofSetColor(color);
	
	ofPushMatrix();
	ofTranslate(s);
	ofSphere(0.1);
	ofPopMatrix();
	
	if (infinite) {
		//'infinite' line
		
		//inner line
		if (f > 2.0f){
			ofPushStyle();
			ofSetLineWidth(1.0f);
			ofSetColor(255,255,255);
			ofLine(s-100*t, s+100*t);
			ofPopStyle();
		}
		
		
		ofSetLineWidth(f==0.0f ? 1.0f : f);
		ofLine(s-1000*t, s+1000*t);
	}
	
	//line section
	ofSetLineWidth(f==0.0f ? 2.0f : f);
	ofLine(s, s+t);
	
	
	ofPopStyle();
}

void Ray::randomise() {
	infinite = true;
	s = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf());
	t = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf());
	
	color = ofColor(150.0f + 100.0f * ofRandomuf(), 150.0f + 100.0f * ofRandomuf(), 150.0f + 100.0f * ofRandomuf(), 255);
}

void Ray::transform(ofMatrix4x4 m) {
	ofVec3f sOld = s;
	s = s * m;
	t = (t+sOld) * m - s;
}

// from http://paulbourke.net/geometry/lineline3d/lineline.c 
//	int LineLineIntersect(
//	XYZ p1,XYZ p2,XYZ p3,XYZ p4,XYZ *pa,XYZ *pb,
//	double *mua, double *mub)
Ray Ray::intersect(Ray &other) {	
	Ray intersectRay(0);
	
	const ofVec3f p1(s), p2(s+t), p3(other.s), p4(other.s+other.t);
	const float EPS(1.0E-5);
	
	ofVec3f p13,p43,p21;
	float d1343,d4321,d1321,d4343,d2121;
	float numer,denom;
	
	p13.x = p1.x - p3.x;
	p13.y = p1.y - p3.y;
	p13.z = p1.z - p3.z;
	p43.x = p4.x - p3.x;
	p43.y = p4.y - p3.y;
	p43.z = p4.z - p3.z;
	if (fabs(p43.x) < EPS && fabs(p43.y) < EPS && fabs(p43.z) < EPS)
		return(Ray(0));
	p21.x = p2.x - p1.x;
	p21.y = p2.y - p1.y;
	p21.z = p2.z - p1.z;
	if (fabs(p21.x) < EPS && fabs(p21.y) < EPS && fabs(p21.z) < EPS)
		return(Ray(0));
	
	d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
	d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
	d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
	d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
	d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;
	
	denom = d2121 * d4343 - d4321 * d4321;
	if (fabs(denom) < EPS)
		return(Ray(0));
	numer = d1343 * d4321 - d1321 * d4343;
	
	const float ma = numer / denom;
	const float mb = (d1343 + d4321 * (ma)) / d4343;
	
	intersectRay.s.x = p1.x + ma * p21.x;
	intersectRay.s.y = p1.y + ma * p21.y;
	intersectRay.s.z = p1.z + ma * p21.z;
	intersectRay.t.x = p3.x + mb * p43.x;
	intersectRay.t.y = p3.y + mb * p43.y;
	intersectRay.t.z = p3.z + mb * p43.z;
	intersectRay.t = intersectRay.t - intersectRay.s;
	
	intersectRay.infinite = false;
	return intersectRay;
}

ofVec3f Ray::getMidpoint() const {
	return s + t*0.5;
}

float Ray::getLength() const {
	return t.length();
}