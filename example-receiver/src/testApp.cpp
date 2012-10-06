#include "testApp.h"

#include "ofxPublishScreen.h"

ofxPublishScreen::Subscriber subs;
ofImage image;

//--------------------------------------------------------------
void testApp::setup()
{
	ofBackground(30);
	
	subs.setup("localhost", 20000);
}

//--------------------------------------------------------------
void testApp::update()
{
	subs.update();
	if (subs.isFrameNew())
	{
		image.setFromPixels(subs.getPixelsRef());
	}
	
	ofSetWindowTitle(ofToString(subs.getFps(), 2));
}

//--------------------------------------------------------------
void testApp::draw()
{
	image.draw(0, 0);
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