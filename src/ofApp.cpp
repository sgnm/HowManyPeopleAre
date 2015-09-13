#include "ofApp.h"
#include <fstream>
#include <iostream>
#include <string>

//--------------------------------------------------------------
void ofApp::setup(){
    ofToggleFullscreen();
    ofBackground(0,0,0);
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ADD); //加算合成すると北極と南極が明るすぎるので無い方が綺麗説
    
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
    
    rollCam.setup();//rollCam's setup.
    rollCam.setCamSpeed(0.1);//rollCam's speed set;
    light.setup();
    ofEnableLighting(); //Lightが使えてかっこいいけど見にくいリリースの時だけ実行する
    light.setPosition(0, 300, 0);
//    light.draw();
//    light.enable();
    
    cam.setPosition(0, 0, 450);
    cam.lookAt(ofVec3f(0, 0, 0));
    
    bgm.loadSound("noise.wav");
    bgm.play();
    bgm.setLoop(true);
    
    // 効果音読み込み
    pi.loadSound("pi.mp3");
    zoomIn.loadSound("zoomIn.mp3");
    zoomOut.loadSound("zoomOut.mp3");
    rotSound.loadSound("rotate.mp3");
    
    for(int i = 0; i < 1; i++){
        for(int x = 0; x < w; x+=1){
            for(int y = 0; y < h; y+=1){
                ofPushMatrix();
                
                // xy座標を極座標変換してあげてる
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
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // jsonデータ取得
    response.open("/Users/Shin/Desktop/stream.json");
    response.getRawString();
    
    rollCam.update();   //rollCam's rotate update.
    
    oldStr = newStr;
    ifstream ifs("/Users/Shin/Desktop/stream.json");
    while (getline(ifs, str)){
        // 次にdraw()が呼ばれたときに、oldStrとの差分を比較するため、読み込んだjsonをnewStrに格納
        newStr = str;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
//    for(int i = 0; i < 1; i++){
//        for(int x = 0; x < w; x+=1){
//            for(int y = 0; y < h; y+=1){
//                ofPushMatrix();
//                
//                // xy座標を極座標変換してあげてる
//                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
//                float theta = (float)y * M_PI / h;
//                float phi = -(float)x * 2.0f * M_PI / w;
//                float px = radius * sin(theta) * cos(phi);
//                float py = radius * sin(theta) * sin(phi);
//                float pz = radius * cos(theta);
//                ofVec3f pos(px, py, pz);
//                mesh.addVertex(pos);
//                
//                ofPopMatrix();
//            }
//        }
//    }
    
    cam.begin();
    // 向きの調整&回転させてる
    rollCam.begin(); //rollCam begin
    
    ofPushMatrix();
    ofRotateX(-90);
    ofRotateZ(ofGetElapsedTimef()*5);
    if (bDrawEarth) {
        mesh.draw();
    }
    ofPopMatrix();
    
//    ofDrawBitmapString(ofToString(ofGetFrameRate(), 0), 20, 20);
                          
    // json内容比較して（更新されたら）、ツイートしたユーザの情報取得
    if (newStr.length() != oldStr.length()) {
        lon = response["place"]["bounding_box"]["coordinates"][0][0][0].asFloat();
        lat = response["place"]["bounding_box"]["coordinates"][0][0][1].asFloat();
        
        // 国とテキスト、user_nameとimage取得
        country = response["place"]["full_name"].asString();
        text = response["text"].asString();
        user_name = response["user"]["name"].asString();
        imgUrl = response["user"]["profile_image_url"].asString();
        userPic.push_back(ofImage(imgUrl));

        City city = {country, lat, lon, text, user_name};
        cities.push_back( city );
        
        // ツイートがあるたびに、音を鳴らす。30.0fは演出の為。
        if(ofGetElapsedTimef() > 10.0f){
            pi.setVolume(0.3f);
            pi.play();
        }
    }
    
    ofSetColor(255);
    for(unsigned int i = 0; i < cities.size(); i++){
//        if(ofGetElapsedTimef() > 10.2f){
            // 位置情報offの時の処理
            if(ofToString(cities[i].latitude) == "0"){
                cities[i].latitude = ofRandom(0, 360);
                cities[i].longitude = ofRandom(0, 360);
            }
        
            oldlatRot.makeRotate(cities[i-1].latitude, 1, 0, 0);
            oldlongRot.makeRotate(cities[i-1].longitude, 0, 1, 0);
            oldspinQuat.makeRotate(ofGetElapsedTimef(), 0, 1, 0);
        
            latRot.makeRotate(cities[i].latitude, 1, 0, 0);
            longRot.makeRotate(cities[i].longitude, 0, 1, 0);
            spinQuat.makeRotate(ofGetElapsedTimef(), 0, 1, 0);
            
            ofVec3f center = ofVec3f(0,0,radius);
            oldWorldPoint = oldlatRot * oldlongRot * oldspinQuat * center;
            worldPoint = latRot * longRot * spinQuat * center;
            
            miniradius = ofRandom(0, 10);
            ofPushStyle();
            ofSetColor(255, ofRandom(0, 255));
            ofDrawSphere(worldPoint, miniradius);
            ofPopStyle();
        
            if (bModeLine) {
                ofLine(ofVec3f(0,0,0), worldPoint);
            }else{
                ofLine(worldPoint, oldWorldPoint);
            }
        
            ofSetColor(255);
            ofDrawBitmapString(cities[i].user_name, worldPoint * 1.2 + ofVec3f(20, 0, 0));
            ofDrawBitmapString(cities[i].text, worldPoint + ofVec3f(20, -20, 0));
            
            userPic[i].draw(worldPoint.x * 1.2, worldPoint.y * 1.2, worldPoint.z * 1.2, userPic[i].width, userPic[i].height);
//        }
    }
    
    rollCam.end();  //rollCam end
    cam.end();
    
    ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 20);
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
        zoomIn.setVolume(0.3f);
        zoomIn.play();
    }
    if (key == '5'){
        rollCam.setScale(1.5);
    }
    if (key == '6') {
        rollCam.setScale(1);
        cam.lookAt(ofVec3f(0, 0, 0));
        if (seven) {
            zoomIn.setVolume(0.3f);
            zoomIn.play();
            seven = false;
        }else{
            zoomOut.setVolume(0.3f);
            zoomOut.play();
        }
    }
    if (key == '7'){
        // デフォルトに戻す
        rollCam.setScale(0.7);
        rollCam.setPos(0, 0, 0);
        cam.setPosition(0, 0, 450);
        cam.lookAt(ofVec3f(0, 0 ,0));
        zoomOut.setVolume(0.3f);
        zoomOut.play();
        seven = true;
    }
    if (key == 'f'){
        ofToggleFullscreen();
    }
    if (key == '8'){
        rollCam.setPos(0, 0, 0);
        cam.lookAt(ofVec3f(0, 0, 0));
    }
    if (key == '9'){
        cam.setPosition(worldPoint * 1.5f);
        cam.lookAt(worldPoint);
    }
    if (key == '0'){
        rollCam.setPos(worldPoint.x, worldPoint.y, worldPoint.z);
        cout << "x: " << worldPoint.x << " y: " << worldPoint.y << " z: " << worldPoint.z << endl;
    }
    if (key == 'l') {
        bLight = !bLight;
        if (bLight) {
            ofEnableLighting();
            light.enable();
            light.draw();
        }else{
            ofDisableLighting();
            light.destroy();
            light.disable();
        }
    }
    if (key == 'd') {
        bModeLine = !bModeLine;
    }
    if (key == 'e') {
        bDrawEarth = !bDrawEarth;
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
