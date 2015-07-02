#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxRollingCam.h"

//typedef struct {
//    string name;
//    float latitude;
//    float longitude;
//    string text;
//    string user_name;
//} City;

class City{

public:
    string country;
    float latitude;
    float longitude;
    string text;
    string user_name;
    
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    static const int NUM_PARTICLES = 20000;
    
    ofImage image, outImage;
    ofVboMesh mesh;
   	vector<City> cities;
    ofVec3f center;
    ofVec3f worldPoint;
    ofxJSONElement response;
    int numloadpic = 0;
    
    int threshold;
    float rad = 0.0;
    
    int w, h;
    int rotate;
    
    float radius = 200;
    float theta, phi;
    
    int imgVec[200];
    int i = 0;
    
    ofColor imgColor[NUM_PARTICLES];
    float intensity[NUM_PARTICLES];
    
    string country;
    string text;
    string user_name;
    string imgUrl;
    ofImage userPic;

    ofxRollingCam rollCam;
    ofLight light;
    
    ofCamera cam;
    
    ofQuaternion latRot, longRot, spinQuat;
    
    string str, oldStr, newStr;
    
};
