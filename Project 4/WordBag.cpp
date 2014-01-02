#include "provided.h"
#include <string>
#include "MyMap.h"
using namespace std;

class WordBagImpl
{
public:
    WordBagImpl(const string& text);
    bool getFirstWord(string& word, int& count);
    bool getNextWord(string& word, int& count);

private:
	MyMap<string, int> m_wordMap;
};

WordBagImpl::WordBagImpl(const string& text)
{
	Tokenizer t(text);
	string w;
	while(t.getNextToken(w))
	{
		strToLower(w);
		int * count = m_wordMap.find(w);

		if(count == NULL)
			m_wordMap.associate(w, 1);
		else
			m_wordMap.associate(w, *count+1);
	}
}

bool WordBagImpl::getFirstWord(string& word, int& count)
{
	int * check = m_wordMap.getFirst(word);

	if(check == NULL)
		return false;
	else
	{
		count = *check;
		return true;
	}
}

bool WordBagImpl::getNextWord(string& word, int& count)
{
    int * check = m_wordMap.getNext(word);

	if (check == NULL)
		return false;
	else
	{
		count = *check;
		return true;
	}
}

//******************** WordBag functions *******************************

// These functions simply delegate to WordBagImpl's functions.
// You probably don't want to change any of this code.

WordBag::WordBag(const string& text)
{
    m_impl = new WordBagImpl(text);
}

WordBag::~WordBag()
{
    delete m_impl;
}

bool WordBag::getFirstWord(string& word, int& count)
{
    return m_impl->getFirstWord(word, count);
}

bool WordBag::getNextWord(string& word, int& count)
{
    return m_impl->getNextWord(word, count);
}
