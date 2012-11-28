#include "ofxPublishScreen.h"

#pragma mark - Publisher

class ofxPublishScreen::Publisher::SenderThread : public ofThread
{
public:
	
	SenderThread()
	{
		
	}
	
	void setup(ofxZmqPublisher *pub_, ofImageFormat format_)
	{
		pub = pub_;
		format = format_;
	}
	
	void pushImage(const ofPixels &pix)
	{
		if (lock())
		{
			frames.push(pix);
			unlock();
		}
	}
	
protected:
	
	ofxZmqPublisher *pub;
	queue<ofPixels, list<ofPixels> > frames;
	ofImageFormat format;
	ofxTurboJpeg jpeg;
	
	float publish_fps;
	float last_published_time;
	
	void threadedFunction()
	{
		while (isThreadRunning())
		{
			while (!frames.empty() && isThreadRunning())
			{
				ofPixels pix;
				
				if (lock())
				{
					pix = frames.front();
					frames.pop();
					unlock();
				}
				
				ofBuffer data;
				jpeg.save(data, pix,  75);
				
				// send timestamp
//				float t = ofGetElapsedTimef();
//				pub->send(&t, sizeof(float), true, true);
				pub->send(data, true);
			}
			
			ofSleepMillis(1);
		}
	}
};

void ofxPublishScreen::Publisher::setup(int port, ofImageFormat format_)
{
	dispose();
	
	char buf[256];
	sprintf(buf, "tcp://*:%i", port);
	
	pub.setHighWaterMark(1);
	pub.bind(buf);
	
	format = format_;
	
	sender_thread = new SenderThread;
	sender_thread->setup(&pub, format);
	sender_thread->startThread();
	
	ofAddListener(ofEvents().exit, this, &Publisher::onExit);
}

void ofxPublishScreen::Publisher::dispose()
{
	if (sender_thread)
	{
		SenderThread *t = sender_thread;
		sender_thread = NULL;
		// t->stopThread(true);
		t->stopThread();
		delete t;
	}
}

ofxPublishScreen::Publisher::~Publisher()
{
	dispose();
}

void ofxPublishScreen::Publisher::publishScreen()
{
	int w = ofGetWidth();
	int h = ofGetHeight();
	
	ofTexture tex;
	tex.allocate(w, h, GL_RGBA);
	
	tex.loadScreenData(0, 0, w, h);
	
	publishTexture(&tex);
	
	tex.clear();
}

void ofxPublishScreen::Publisher::publishPixels(const ofPixels &pix)
{
	sender_thread->pushImage(pix);
}

void ofxPublishScreen::Publisher::publishTexture(ofTexture* inputTexture)
{
	ofPixels pix;
	inputTexture->readToPixels(pix);
	publishPixels(pix);
}

void ofxPublishScreen::Publisher::onExit(ofEventArgs&)
{
	dispose();
}

#pragma mark - Subscriber

class ofxPublishScreen::Subscriber::ReceiverThread : public ofThread
{
public:
	
	ofxZmqSubscriber *subs;
	ofPixels pix;
	ofxTurboJpeg jpeg;
	
	bool has_new_pixels;
	
	void setup(ofxZmqSubscriber *subs_)
	{
		has_new_pixels = false;
		subs = subs_;
	}
	
	void threadedFunction()
	{
		while (isThreadRunning())
		{
			while (subs->hasWaitingMessage())
			{
				ofBuffer data;
				subs->getNextMessage(data);
				
				ofPixels temp;
				if (jpeg.load(data, temp))
				{
					if (lock())
					{
						pix = temp;
						has_new_pixels = true;
						unlock();
					}
				}
			}
			
			ofSleepMillis(1);
		}
	}

};

void ofxPublishScreen::Subscriber::setup(string host, int port)
{
	dispose();
	
	char buf[256];
	sprintf(buf, "tcp://%s:%i", host.c_str(), port);
	
	subs.setHighWaterMark(2);
	subs.connect(buf);
	
	receiver_thread = new ReceiverThread;
	receiver_thread->setup(&subs);
	receiver_thread->startThread();
	
	last_subs_time = 0;
	subs_fps = 0;
}

void ofxPublishScreen::Subscriber::dispose()
{
	if (receiver_thread)
	{
		delete receiver_thread;
		receiver_thread = NULL;
	}
}

void ofxPublishScreen::Subscriber::update()
{
	if (receiver_thread->lock())
	{
		if (receiver_thread->has_new_pixels)
		{
			receiver_thread->has_new_pixels = false;
			image.setFromPixels(receiver_thread->pix);
			
			float d = ofGetElapsedTimef() - last_subs_time;
			d = 1. / d;
			
			subs_fps += (d - subs_fps) * 0.1;
			
			last_subs_time = ofGetElapsedTimef();
		}
		receiver_thread->unlock();
	}
}

void ofxPublishScreen::Subscriber::draw(int x, int y)
{
	if (image.isAllocated())
		image.draw(x, y);
}
