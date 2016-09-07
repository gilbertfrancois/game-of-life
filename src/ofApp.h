//
// Created by Gilbert Francois on 05-09-16.
//
#pragma once

#include "ofMain.h"
#include "GameOfLifeKernel.h"

class ofApp : public ofBaseApp{
    
private:
    int rows = 20;
    int cols = 30;
    bool withThreads = true;
    bool withTextureBuffer = true;
    bool withPixelSize = true;
    bool bMutate = false;
    GameOfLifeKernel *gofKernel;
    int **X;
    
    int windowWidth;
    int windowHeight;
    ofImage img;
    
    void drawCross(int x, int y);
    
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
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
};
