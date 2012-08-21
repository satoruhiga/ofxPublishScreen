#include "testApp.h"

#include "ofxPublishScreen.h"

ofxPublishScreen::Publisher pub;

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	ofBackground(30);
	
	pub.setup(20000, 1024, 768);
}

//--------------------------------------------------------------
void testApp::update()
{
	pub.begin();
	ofClear(ofRandom(255), ofRandom(255), ofRandom(255));
	ofCircle(ofGetMouseX(), ofGetMouseY(), 100);
	pub.end();
}

//--------------------------------------------------------------
void testApp::draw()
{
	pub.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

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