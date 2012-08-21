#pragma once

#include "ofMain.h"
#include "ofxZmq.h"

namespace ofxPublishScreen {
	
	class Publisher
	{
	public:
		
		void setup(int port, int w, int h, int internalformat = GL_RGB)
		{
			fbo.allocate(w, h, internalformat);
			
			char buf[256];
			sprintf(buf, "tcp://*:%i", port);
			
			pub.setHighWaterMark(2);
			pub.bind(buf);
			
			format = OF_IMAGE_FORMAT_TIFF;
		}
		
		void draw(int x = 0, int y = 0)
		{
			fbo.draw(x, y);
		}
		
		void begin()
		{
			fbo.begin();
			ofFloatColor bg = ofGetCurrentRenderer()->getBgColor();
			ofClear(bg.r * 255, bg.g * 255, bg.b * 255);
		}
		
		void end()
		{
			fbo.end();
			
			ofPixels pix;
			fbo.readToPixels(pix);
			
			ofBuffer data;
			
			// TODO: image compression on another thread
			ofSaveImage(pix, data, format);
			pub.send(data);
		}
		
		float getWidth() { return fbo.getWidth(); }
		float getHeight() { return fbo.getHeight(); }
		
		void setImageCompressionFormat(ofImageFormat v) { format = v; }
		
	protected:
		
		ofxZmqPublisher pub;
		ofFbo fbo;
		
		ofImageFormat format;
	};
	
	class Subscriber
	{
	public:
		
		void setup(string host, int port)
		{
			subs.setHighWaterMark(2);
			
			char buf[256];
			sprintf(buf, "tcp://%s:%i", host.c_str(), port);
			
			subs.connect(buf);
		}
		
		void update()
		{
			while (subs.hasWaitingMessage())
			{
				ofBuffer data;
				subs.getNextMessage(data);
				image.loadImage(data);
			}
		}
		
		void draw(int x = 0, int y = 0)
		{
			image.draw(x, y);
		}
		
		ofImage& getImage() { return image; }
		
	protected:
		
		ofImage image;
		ofxZmqSubscriber subs;
		
	};
	
}
