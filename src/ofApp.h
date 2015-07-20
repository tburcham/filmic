#pragma once

#include "ofMain.h"
#include "ofxFFTLive.h"
#include "ofxGui.h"

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
    

    
        ofxFFTLive fftLive;
        
        ofxPanel gui;
        ofxSlider<float> audioThreshold;
        ofxSlider<float> audioPeakDecay;
        ofxSlider<float> audioMaxDecay;
        ofxToggle audioMirror;
        
        ofMesh meshOriginal;
        ofMesh meshWarped;

    
        ofVideoPlayer video; //Prerecorded video
        ofFbo fbo;
        ofMesh mesh;
        ofImage image;
        ofPixels fboPixels;
        ofShader shader;
        ofEasyCam easyCam;
    
        private:
    
            int videoWidth = 320;
            int videoHeight = 568;
    
            float factor = 0.5;
    
            int W = 100 * factor; //Grid size
            int H = 100 * factor;
            int meshSize = 6 / factor;
    
    
		
};
