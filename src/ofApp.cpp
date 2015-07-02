#include "ofApp.h"
#include <fstream>
#include <iostream>
#include <string>

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ADD); //加算合成すると北極と南極が明るすぎるので無い方が綺麗
    
    image.loadImage("world.png");
    image.resize(200, 100);
    w = image.width;
    h = image.height;
    image.allocate(w, h, OF_IMAGE_COLOR);
    
    // 海と大陸の閾値設定
    threshold = 140;
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableColors();
    glPointSize(3);
    glEnable(GL_DEPTH_TEST);
    
    for(int x = 0; x < w; x+=1){
        for(int y = 0; y < h; y+=1){
            // 画像から色と明るさ取得
            imgColor[i] = image.getColor(x, y);
            intensity[i] = imgColor[i].getLightness();
            if (intensity[i] < threshold) {
                // 明るさの閾値より低い時、青を表示
                mesh.addColor(ofColor::fromHsb(150, 255, 255));
            }else{
                mesh.addColor(ofColor::fromHsb(120, 255, 255));
            }
            i++;
        }
    }
    
    userPic.allocate(100, 100, OF_IMAGE_COLOR);
    
    rollCam.setup();//rollCam's setup.
    rollCam.setCamSpeed(0.1);//rollCam's speed set;
    light.setup();
    ofEnableLighting(); //Lightが使えてかっこいいけど見にくいリリースの時だけ実行する
    light.setPosition(0, 300, 0);
    light.draw();
    light.enable();
    
    cam.setPosition(0, 0, 450);
    
    bgm.loadSound("bgm.mp3");
    bgm.play();
    
    pi.loadSound("pi.mp3");
    zoomIn.loadSound("zoomIn.mp3");
    zoomOut.loadSound("zoomOut.mp3");
    rotSound.loadSound("rotate.mp3");
    
}

//--------------------------------------------------------------
void ofApp::update(){
    mesh.clearVertices();
    
    // jsonデータ取得
    response.open("/Users/Shin/Desktop/stream.json");
    response.getRawString();
    
    rollCam.update();   //rollCam's rotate update.
    
    oldStr = newStr;
    ifstream ifs("/Users/Shin/Desktop/stream.json");
    while (getline(ifs, str))
    {
        //        cout << str << endl;
        newStr = str;
    }
//    cout << str.length() << endl;
//    cout << newStr.length() << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float radius = 200;
    
    for(int i = 0; i < 1; i++){
        for(int x = 0; x < w; x+=1){
            for(int y = 0; y < h; y+=1){
                
                ofPushMatrix();
                
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                float theta = (float)y * M_PI / h;
                float phi = -(float)x * 2.0f * M_PI / w;
                float px = radius * sin(theta) * cos(phi);
                float py = radius * sin(theta) * sin(phi);
                float pz = radius * cos(theta);
                ofVec3f pos(px, py, pz);
                mesh.addVertex(pos);
                
                ofPopMatrix();
            }
        }
    }
    cam.begin();
    // 向きの調整&回転させてる
    rollCam.begin(); //rollCam begin
    
    ofPushMatrix();
    ofRotateX(-90);
    ofRotateZ(ofGetElapsedTimef()*5);
    mesh.draw();
    ofPopMatrix();
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 0), 20, 20);
    
    // ツイートの位置情報取得
    if (newStr.length() != oldStr.length()) {
        lon = response["place"]["bounding_box"]["coordinates"][0][0][0].asFloat();
        lat = response["place"]["bounding_box"]["coordinates"][0][0][1].asFloat();
        
        // 国とテキスト、user_nameとimage取得
        country = response["place"]["full_name"].asString();
        text = response["text"].asString();
        user_name = response["user"]["name"].asString();
        imgUrl = response["user"]["profile_image_url"].asString();
//        userPic.loadImage(imgUrl);
        
        City city = {country, lat, lon, text, user_name};
        cities.push_back( city );
        if(ofGetElapsedTimef() > 25.0f){
            pi.setVolume(0.1f);
            pi.play();
        }
    }
    
    ofSetColor(255);
    for(unsigned int i = 0; i < cities.size(); i++){
        if(ofGetElapsedTimef() > 22.2f){
            // 位置情報offの時の処理
            if(ofToString(cities[i].latitude) == "0"){
                cities[i].latitude = ofRandom(0, 360);
                cities[i].longitude = ofRandom(0, 360);
            }
            
            latRot.makeRotate(cities[i].latitude, 1, 0, 0);
            longRot.makeRotate(cities[i].longitude, 0, 1, 0);
            spinQuat.makeRotate(ofGetElapsedTimef(), 0, 1, 0);
            
            ofVec3f center = ofVec3f(0,0,radius);
            worldPoint = latRot * longRot * spinQuat * center;
            
            miniradius = ofRandom(0, 10);
            ofPushStyle();
            ofSetColor(255, ofRandom(0, 255));
            ofDrawSphere(worldPoint, miniradius);
            ofPopStyle();
            ofLine(ofVec3f(0,0,0), worldPoint);
            
    //        userPic.draw(worldPoint * 1.2, userPic.width, userPic.height);
            ofDrawBitmapString(cities[i].user_name, worldPoint * 1.2);
            ofDrawBitmapString(cities[i].text, worldPoint * 1.2 - 20);
            ofDrawBitmapString(cities[i].country, worldPoint * 1.2 - 40);
        }
    }
    
    rollCam.end();  //rollCam end
    cam.end();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '1') {//Inputting optional rotate.
        rollCam.setPos(0, 0, 0);
    }
    if (key == '2') {//Random rotate.
        cam.lookAt(ofVec3f(0, 0, 0));
        rollCam.setScale(1);
        rollCam.setRandomPos(270);
        rotSound.setVolume(0.3f);
        rotSound.play();
    }
    if (key == '3') {//Inputting optional rotate.
        rollCam.setPos(0, 0, 0);
        cam.lookAt(worldPoint);
        rollCam.setScale(1);
        pi.setVolume(0.5f);
        pi.play();
    }
    if (key == '4'){
        rollCam.setScale(2);
        zoomIn.play();
    }
    if (key == '5'){
        rollCam.setScale(1.5);
    }
    if (key == '6') {
        rollCam.setScale(1);
        cam.lookAt(ofVec3f(0, 0, 0));
        zoomOut.play();
    }
    if (key == 'f'){
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
