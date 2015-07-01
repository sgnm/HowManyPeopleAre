#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    //    ofEnableBlendMode(OF_BLENDMODE_ADD); //加算合成すると北極と南極が明るすぎるので無い方が綺麗
    
    image.loadImage("world.png");
    image.resize(200, 100);
    w = image.width;
    h = image.height;
    image.allocate(w, h, OF_IMAGE_COLOR);
    
    // 海と大陸の閾値設定
    threshold = 140;
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableColors();
    glPointSize(1.5);
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
    
   	City tokyo = { "tokyo", 35 + 40./60, 139 + 45/60. };
   	cities.push_back( tokyo );
    
    userPic.allocate(100, 100, OF_IMAGE_COLOR);
    
    rollCam.setup();//rollCam's setup.
    rollCam.setCamSpeed(0.1);//rollCam's speed set;
}

//--------------------------------------------------------------
void ofApp::update(){
    mesh.clearVertices();
    //    ofRotateY(rotate);
    rotate += 1;
    
    // jsonデータ取得
    response.open("/Users/Shin/Desktop/stream.json");
    response.getRawString();
    
    rollCam.update();   //rollCam's rotate update.
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    rollCam.begin(); //rollCam begin
    
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
    
    // 向きの調整&回転させてる
    ofRotateX(90);
    ofRotateZ(ofGetElapsedTimef()*5);
    mesh.draw();
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 0), 20, 20);
    
    // ツイートの位置情報取得
        float lon = response["place"]["bounding_box"]["coordinates"][0][0][0].asFloat();
        float lat = response["place"]["bounding_box"]["coordinates"][0][0][1].asFloat();
//        cout << ofToString(lon, 12) + " " + ofToString(lat, 12) << endl;
        // 国とテキスト、user_nameとimage取得
        country = response["place"]["full_name"].asString();
        text = response["text"].asString();
        user_name = response["user"]["name"].asString();
        imgUrl = response["user"]["profile_image_url"].asString();
        //        userPic.loadImage(imgUrl);
    
    //    City newyork = { "new york", 40+47/60., -73 + 58/60. };
    
    ofSetColor(255);
    for(unsigned int i = 0; i < cities.size(); i++){
        
        ofQuaternion latRot, longRot, spinQuat;
        latRot.makeRotate(cities[i].latitude, 1, 0, 0);
        longRot.makeRotate(cities[i].longitude, 0, 1, 0);
        spinQuat.makeRotate(ofGetElapsedTimef(), 0, 1, 0);
        
        ofVec3f center = ofVec3f(0,0,radius);
        worldPoint = latRot * longRot * spinQuat * center;
        cout << worldPoint.x << endl;
        
        ofLine(ofVec3f(0,0,0), worldPoint);
        
        //        userPic.draw(worldPoint, userPic.width, userPic.height);
        //        ofDrawBitmapString(country, worldPoint + 10);
        ofDrawBitmapString(user_name, worldPoint + 50);
        //        ofDrawBitmapString(text, worldPoint + 100);
        //        ofDrawBitmapString(country, worldPoint + 150);
    }
    
    rollCam.end();  //rollCam end
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='1') {//Inputting optional rotate.
        rollCam.setPos(0, 0, 0);
    }
    if (key=='2') {//Random rotate.
        rollCam.setScale(1);
        rollCam.setRandomPos(270);
    }
    if (key=='3') {//Inputting optional rotate.
        rollCam.setScale(2);
        rollCam.setPos(worldPoint.x,worldPoint.y, worldPoint.z);
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
