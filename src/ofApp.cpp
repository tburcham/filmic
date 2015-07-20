#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofEnableAlphaBlending();
    
    fftLive.setMirrorData(false);
    fftLive.setup();
    
    string guiPath = "audio.xml";
    gui.setup("audio", guiPath, 20, 20);
    gui.add(audioThreshold.setup("audioThreshold", 1.0, 0.0, 1.0));
    gui.add(audioPeakDecay.setup("audioPeakDecay", 0.915, 0.0, 1.0));
    gui.add(audioMaxDecay.setup("audioMaxDecay", 0.995, 0.0, 1.0));
    gui.add(audioMirror.setup("audioMirror", true));
    gui.loadFromFile(guiPath);

    
    shader.load("shaders_gl3/noise");
    
    video.loadMovie( "elise.mov" ); //Load the video file
    video.play(); //Start the video
    
    mesh = ofMesh();
    
    
    //setup fbo
    fbo.allocate(videoWidth, videoHeight, GL_RGBA);
    // clear fbo
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    //mesh.setMode(OF_PRIMITIVE_POINTS);
    
    
    //Set up vertices
    for (int y=0; y<H; y++) {
        for (int x=0; x<W; x++) {
            mesh.addVertex(ofPoint((x - W/2) * meshSize, (y - H/2) * meshSize, 0 )); // adding texure coordinates allows us to bind textures to it later // --> this could be made into a function so that textures can be swapped / updated
            mesh.addTexCoord(ofPoint(x * (videoWidth / W), y * (videoHeight / H)));
            mesh.addColor(ofColor(255, 255, 255, 0));
        }
    }
    
    //Set up triangles' indices
    for (int y=0; y<H-1; y++) {
        for (int x=0; x<W-1; x++) {
            int i1 = x + W * y;
            int i2 = x+1 + W * y;
            int i3 = x + W * (y+1);
            int i4 = x+1 + W * (y+1);
            mesh.addTriangle( i1, i2, i3 );
            mesh.addTriangle( i2, i4, i3 );
        }
    }
    
    
    meshWarped = mesh;
    
    easyCam.setDistance(1000);
    
    easyCam.setNearClip(0);
    easyCam.setFarClip(10000);
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    fftLive.setThreshold(audioThreshold);
    fftLive.setPeakDecay(audioPeakDecay);
    fftLive.setMaxDecay(audioMaxDecay);
    fftLive.setMirrorData(audioMirror);
    fftLive.update();
    
    
    
    video.update();
    
    //convert fbo to ofImage format
    fbo.readToPixels(fboPixels);
    image.setFromPixels(fboPixels);
    
    //Change vertices
    for (int y=0; y<H; y++) {
        for (int x=0; x<W; x++) {
            
            //Vertex index
            int i = x + W * y;
            ofPoint p = mesh.getVertex( i );
            
            float scaleX = videoWidth / W;
            float scaleY = videoHeight / H;
            
            // get brightness
            int index = ((x * scaleX) + videoWidth * (y * scaleY)) * 4; // FBO has four components (including Alpha)
            float brightness = (fboPixels[index] * 2); // 4 is an arbitrary scalar to reduce the amount of distortion
            
            //printf("z value is %f\n", brightness / 255.0);
            //printf("Maximum number of output vertices support is: %i\n", shader.getGeometryMaxOutputCount());
            
            //Change z-coordinate of vertex
            p.z = brightness; // ofNoise(x * 0.05, y * 0.05, ofGetElapsedTimef() * 0.5) * 100;
            mesh.setVertex( i, p );
            
            float alphaThreshold = 255 - (brightness * 1);
            /*if (alphaThreshold < 150) {
                alphaThreshold = 0;
            }*/
            
            //Change color of vertex
            mesh.setColor(i , ofColor(255, 255, 255, alphaThreshold));
            
            meshWarped.setColor(i , ofColor(255, 255, 255, alphaThreshold));
        }
    }
    
    
    //---------------------------------------------------------- dispacing mesh using audio.
    vector<ofVec3f> & vertsOriginal = mesh.getVertices();
    vector<ofVec3f> & vertsWarped = meshWarped.getVertices();
    int numOfVerts = mesh.getNumVertices();
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    
    float meshDisplacement = 250;
    
    for(int i=0; i<numOfVerts; i++) {
        float audioValue = audioData[i];
        ofVec3f & vertOriginal = vertsOriginal[i];
        ofVec3f & vertWarped = vertsWarped[i];
        
        ofVec3f direction = vertOriginal.getNormalized();
        vertWarped = vertOriginal + direction * meshDisplacement * audioValue;
    }
    
    delete[] audioData;
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    //Set background to black and rendering color to white
    ofBackground(0);
    ofSetHexColor(0xffffff);
    
    int w = OFX_FFT_WIDTH;
    int h = OFX_FFT_HEIGHT;
    int x = 20;
    int y = ofGetHeight() - h - 20;
    //fftLive.draw(x, y, w, h);
    
    gui.draw();
    
    
    
    //video.draw(0, 0);
    
    if (video.isFrameNew()) {
        // draw video into fbo
        fbo.begin();
        
        int alpha = 50; // amount of smoothing
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, alpha);
        
        // draw image into fbo
        video.draw(0, 0);
        ofDisableAlphaBlending();
        fbo.end();
    }
    
    //fbo.draw(0, 0);
    
    easyCam.begin();
    
        ofPushMatrix(); //Store the coordinate system
    
        //Move the coordinate center to screen's center
        ofTranslate( ofGetWidth()/2, ofGetHeight()/2, 0 );
        //Draw mesh
    
        image.bind();
    
    
        //mesh.draw();
        meshWarped.draw();
        image.unbind();
        ofEnableAlphaBlending();
    
        //shader.begin();
        meshWarped.drawWireframe();
        //meshWarped.drawVertices();
    //mesh.drawFaces();
        //shader.end();

        ofDisableAlphaBlending();
        ofPopMatrix(); //Restore the coordinate system
    easyCam.end();
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
