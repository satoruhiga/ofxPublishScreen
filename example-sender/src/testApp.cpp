#include "testApp.h"

#include "ofxPublishScreen.h"

ofColor color;

ofxPublishScreen::FboPublisher pub;

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	ofBackground(30);
	
	pub.setup(20000, 1280, 720);
}

//--------------------------------------------------------------
void testApp::update()
{
	pub.begin();
	ofClear(color.r, color.g, color.b);
	ofCircle(ofGetMouseX(), ofGetMouseY(), 100);
	pub.end();
	
	ofSetWindowTitle(ofToString(pub.getFps(), 2));
}

//--------------------------------------------------------------
void testApp::draw()
{
	pub.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	color.r = ofRandom(255);
	color.g = ofRandom(255);
	color.b = ofRandom(255);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}