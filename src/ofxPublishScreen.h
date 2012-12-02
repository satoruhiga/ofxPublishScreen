#pragma once

#include "ofMain.h"
#include "ofxZmq.h"
#include "ofxTurboJpeg.h"

namespace ofxPublishScreen {
	
	class Publisher
	{
	public:
		
		Publisher() : sender_thread(NULL) {}
		virtual ~Publisher();
		
		void setup(int port, ofImageFormat format_ = OF_IMAGE_FORMAT_BMP);
		void dispose();
		
		void publishScreen();
		
		void publishPixels(const ofPixels &pix);
		void publishTexture(ofTexture* inputTexture);
		
		void setImageCompressionFormat(ofImageFormat v) { format = v; }
		
		void onExit(ofEventArgs&);
		
	protected:
		
		ofImageFormat format;
		ofxZmqPublisher pub;
		
		class SenderThread;
		SenderThread *sender_thread;
	};
	
	class FboPublisher : public Publisher
	{
	public:
		
		void setup(int port, int w, int h, int internalformat = GL_RGB, ofImageFormat format = OF_IMAGE_FORMAT_BMP)
		{
			ofFbo::Settings s = ofFbo::Settings();
			s.width = w;
			s.height = h;
			s.internalformat = internalformat;
			s.useDepth = true;
			fbo.allocate(s);
			
			Publisher::setup(port, format);
		}

		void draw(int x = 0, int y = 0)
		{
			fbo.draw(0, 0);
		}
		
		void begin()
		{
			fbo.begin(false);
			ofFloatColor bg = ofGetCurrentRenderer()->getBgColor();
			ofClear(bg.r * 255, bg.g * 255, bg.b * 255);
		}
		
		void end()
		{
			fbo.end();
			publishTexture(&fbo.getTextureReference());
		}

		float getWidth() { return fbo.getWidth(); }
		float getHeight() { return fbo.getHeight(); }
		
	protected:
		
		ofFbo fbo;
	};
	
	class Subscriber
	{
	public:
		
		Subscriber() : receiver_thread(NULL) {}
		
		void setup(string host, int port);
		void dispose();
		
		void update();
		
		void draw(int x = 0, int y = 0);
		
		ofImage& getImage() { return image; }
		
	protected:
		
		ofImage image;
		ofxZmqSubscriber subs;
		float last_subs_time;
		float subs_fps;
		
		class ReceiverThread;
		ReceiverThread *receiver_thread;
	};
	
}
