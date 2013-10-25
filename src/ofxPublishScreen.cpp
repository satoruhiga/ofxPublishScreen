#include "ofxPublishScreen.h"

#pragma mark - Publisher

typedef ofPtr<ofPixels> PixelsRef;

class ofxPublishScreen::Publisher::Thread : public ofThread
{
public:

	Thread(string host, int jpeg_quality) : last_pubs_time(0), pubs_fps(0), compress_time_ms(0), jpeg_quality(jpeg_quality)
	{
		pub.setHighWaterMark(1);
		pub.bind(host);
	}

	void pushImage(const ofPixels &pix)
	{
		PixelsRef p = PixelsRef(new ofPixels(pix));
		
		if (lock())
		{
			frames.push(p);
			unlock();
		}
	}

	float getFps() { return pubs_fps; }
	
	int getJpegQuality() { return jpeg_quality; }
	void setJpegQuality(int v) { jpeg_quality = v; }

protected:
	
	ofxZmqPublisher pub;
	ofxTurboJpeg jpeg;
	
	queue<PixelsRef> frames;

	int jpeg_quality;
	
	float pubs_fps;
	float last_pubs_time;
	
	float compress_time_ms;
	
	void threadedFunction()
	{
		while (isThreadRunning())
		{
			while (isThreadRunning())
			{
				lock();
				if (frames.empty())
				{
					sleep(1);
					unlock();
					continue;
				}
				
				PixelsRef pix = frames.front();
				frames.pop();
				unlock();

				ofBuffer data;

				{
					float comp_start = ofGetElapsedTimeMillis();
					jpeg.save(data, *pix.get(),  jpeg_quality);
					float d = ofGetElapsedTimeMillis() - comp_start;
					compress_time_ms += (d - compress_time_ms) * 0.1;
				}
				
				pub.send(data, true);
				
				float t = ofGetElapsedTimef();
				float d = t - last_pubs_time;
				d = 1. / d;
				
				pubs_fps += (d - pubs_fps) * 0.1;
				last_pubs_time = t;
			}
		}
	}
};

void ofxPublishScreen::Publisher::setup(int port, int jpeg_quality)
{
	dispose();

	char buf[256];
	sprintf(buf, "tcp://*:%i", port);

	thread = new Thread(buf, jpeg_quality);
	thread->startThread();
	
	ofAddListener(ofEvents().exit, this, &Publisher::onExit);
}

void ofxPublishScreen::Publisher::dispose()
{
	if (thread)
	{
		Thread *t = thread;
		thread = NULL;
		t->waitForThread(true);
		delete t;
	}
}

void ofxPublishScreen::Publisher::publishScreen()
{
	int w = ofGetWidth();
	int h = ofGetHeight();

	ofTexture tex;
	tex.getTextureData().bFlipTexture = false;
	tex.allocate(w, h, GL_RGB);
	tex.loadScreenData(0, 0, w, h);

	publishTexture(&tex);

	tex.clear();
}

void ofxPublishScreen::Publisher::publishPixels(const ofPixels &pix)
{
	thread->pushImage(pix);
}

void ofxPublishScreen::Publisher::publishTexture(ofTexture* inputTexture)
{
	ofPixels pix;
	inputTexture->readToPixels(pix);
	publishPixels(pix);
}

int ofxPublishScreen::Publisher::getJpegQuality()
{
	return thread->getJpegQuality();
}

void ofxPublishScreen::Publisher::setJpegQuality(int v)
{
	thread->setJpegQuality(v);
}

void ofxPublishScreen::Publisher::onExit(ofEventArgs&)
{
	dispose();
}

float ofxPublishScreen::Publisher::getFps()
{
	return thread->getFps();
}

#pragma mark - Subscriber

class ofxPublishScreen::Subscriber::Thread : public ofThread
{
public:
	
	ofxZmqSubscriber subs;
	ofPixels pix;
	ofxTurboJpeg jpeg;
	
	bool is_frame_new;
	float last_subs_time;
	float subs_fps;
	
	Thread(string host) : is_frame_new(false), last_subs_time(0), subs_fps(0)
	{
		subs.setHighWaterMark(1);
		subs.connect(host);
	}

	void threadedFunction()
	{
		while (isThreadRunning())
		{
			ofBuffer data;
			
			while (subs.hasWaitingMessage())
			{
				subs.getNextMessage(data);
			}
			
			if (data.size())
			{
				ofPixels temp;
				if (jpeg.load(data, temp))
				{
					lock();
					pix = temp;
					is_frame_new = true;
					unlock();
					
					float d = ofGetElapsedTimef() - last_subs_time;
					d = 1. / d;
					
					subs_fps += (d - subs_fps) * 0.1;
					last_subs_time = ofGetElapsedTimef();
				}
			}
			
			ofSleepMillis(1);
		}
	}
	
	float getFps()
	{
		return subs_fps;
	}

};

void ofxPublishScreen::Subscriber::setup(string host, int port)
{
	dispose();

	char buf[256];
	sprintf(buf, "tcp://%s:%i", host.c_str(), port);
	
	thread = new Thread(buf);
	thread->startThread();
}

void ofxPublishScreen::Subscriber::dispose()
{
	if (thread)
	{
		Thread *t = thread;
		thread = NULL;
		t->waitForThread(true);
		delete t;
	}
}

void ofxPublishScreen::Subscriber::update()
{
	is_frame_new = false;
	
	if (thread->lock())
	{
		if (thread->is_frame_new)
		{
			thread->is_frame_new = false;
			pix = thread->pix;
			
			is_frame_new = true;
		}
		thread->unlock();
	}
}

float ofxPublishScreen::Subscriber::getFps()
{
	return thread->getFps();	
}
