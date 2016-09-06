#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    windowWidth = ofGetWindowWidth();
    windowHeight = ofGetWindowHeight();
    
    if (withPixelSize) {
        rows = windowHeight/2;
        cols = windowWidth/2;
    }
    
    gofKernel = new GameOfLifeKernel(rows, cols, withThreads);
    X = gofKernel->getXt0();
    
    
    img.allocate(cols, rows, OF_IMAGE_COLOR);
    img.setColor(ofColor::white);
    if (rows < (windowHeight / 2)) {
        img.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    gofKernel->timeStep();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (withTextureBuffer) {
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                float pxl = X[i][j]*255;
                ofColor color= ofColor(pxl, 255);
                img.setColor(j, i, color);
            }
        }
        img.update();
        img.draw(0, 0, windowWidth, windowHeight);
    }
    else {
        
        ofBackground(0, 0, 0);
        float cellWidth = (float) windowWidth / cols;
        float cellHeight = (float) windowHeight / rows;
        float halfCellWidth = cellWidth / 2;
        float halfCellHeight = cellHeight / 2;
        for (int i=0; i<rows; i++) {
            for (int j=0; j<cols; j++) {
                if (X[i][j] == 1) {
                    ofSetColor(255, 255, 255);
                    ofFill();
                    float x0 = (float) j * cellWidth + halfCellWidth;
                    float y0 = (float) i * cellHeight + halfCellHeight;
                    ofDrawCircle(x0, y0, halfCellHeight);
                    
                }
            }
        }
    }
}

void ofApp::drawCross(int x, int y) {
    int i = (int)round(ofMap(y, 0, ofGetWindowHeight(), 0, rows));
    int j = (int)round(ofMap(x, 0, ofGetWindowWidth(), 0, cols));
    for (int k = -3; k<=3; k++) {
        
        X[(int)ofClamp(i+k, 0, rows-1)][(int)ofClamp(j+k, 0, cols-1)] = 1;
        X[(int)ofClamp(i-k, 0, rows-1)][(int)ofClamp(j+k, 0, cols-1)] = 1;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    drawCross(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    drawCross(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    windowWidth = ofGetWindowWidth();
    windowHeight = ofGetWindowHeight();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
