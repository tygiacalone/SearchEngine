#include "http.h"
#include "provided.h"
#include <string>
#include <queue>
#include <iostream>
#include "MyMap.h"
using namespace std;

//void reportStatus(string url, bool success)
//{
//	if (success)
//		cout << "Downloaded and indexed the page at " << url << endl; 
//	else 
//		cout << "Unable to download the page at " << url << endl;
//}



class WebCrawlerImpl
{
public:
	void addUrl(string url);
	int getNumberOfUrls() const;
	void crawl(void (*callback)(string url, bool success));
	bool save(string filenameBase);
	bool load(string filenameBase);

private:
	queue<string> m_urlHolder;
	Indexer m_urlIndex;
};

void WebCrawlerImpl::addUrl(string url)
{
	m_urlHolder.push(url);
}

int WebCrawlerImpl::getNumberOfUrls() const
{
	return m_urlHolder.size();  // This compiles, but may not be correct
}

void WebCrawlerImpl::crawl(void (*callback)(string url, bool success))
{
	string url;
	string text;
	bool success;

	while (!m_urlHolder.empty())
	{
		url = m_urlHolder.front();
		m_urlHolder.pop();

		success = HTTP().get(url, text);
		//HTTP().set(url, text);
		if (success)
		{
			WordBag wb(text);
			m_urlIndex.incorporate(url, wb);
		}
		callback(url, success);
	}

}

bool WebCrawlerImpl::save(string filenameBase)
{
	return m_urlIndex.save(filenameBase);  // This compiles, but may not be correct
}

bool WebCrawlerImpl::load(string filenameBase)
{
	return m_urlIndex.load(filenameBase);  // This compiles, but may not be correct
}

//******************** WebCrawler functions *******************************

// These functions simply delegate to WebCrawlerImpl's functions.
// You probably don't want to change any of this code.

WebCrawler::WebCrawler()
{
	m_impl = new WebCrawlerImpl;
}

WebCrawler::~WebCrawler()
{
	delete m_impl;
}

void WebCrawler::addUrl(string url)
{
	m_impl->addUrl(url);
}

int WebCrawler::getNumberOfUrls() const
{
	return m_impl->getNumberOfUrls();
}

void WebCrawler::crawl(void (*callback)(string url, bool success))
{
	m_impl->crawl(callback);
}

bool WebCrawler::save(string filenameBase)
{
	return m_impl->save(filenameBase);
}

bool WebCrawler::load(string filenameBase)
{
	return m_impl->load(filenameBase);
}
