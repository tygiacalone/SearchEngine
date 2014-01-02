#include "provided.h"
#include <string>
#include <vector>
#include "MyMap.h"
#include <iostream>
#include <fstream>
using namespace std;

	struct IdCount
	{

		IdCount(){id = 0; count = 0;}
		IdCount(int passedId, int passedCount){id = passedId; count = passedCount;}

		int id;
		int count;
	};

class IndexerImpl
{
public:
	bool incorporate(string url, WordBag& wb);
	vector<UrlCount> getUrlCounts(string word);
	bool save(string filenameBase);
	bool load(string filenameBase);


private:

	//Returns unique ID. If already exists, returns existing ID.
	int getUrlId(string url);

	//<Keytype, ValueType>
	MyMap<string, int> m_urlToId;
	MyMap<int, string> m_idToUrl;
	MyMap<string, vector<IdCount>> m_wordToIdCounts; //<string, (int id, int count)>

};

int IndexerImpl::getUrlId(string url)
{
	int id = rand();
	int * existingId = NULL;

	existingId = m_urlToId.find(url);

	//Check to see if url already exists. If so, return its unique ID
	if (existingId != NULL)
		return *existingId;


	//If it doesn't exist, create a unique ID and return the ID.
	for(;;)
	{
		//Don't leave loop until a unique ID is generated.
		if (m_idToUrl.find(id) == NULL)
			break;

		id = rand();
	}

	return id;
}

bool IndexerImpl::incorporate(string url, WordBag& wb)
{
	//Check to see if URL has already been incorporated.
	strToLower(url);

	if (m_urlToId.find(url) != NULL)
		return false;	

	//Add URL with unique ID.
	int id;
	int count;

	id = getUrlId(url);

	m_idToUrl.associate(id, url);
	m_urlToId.associate(url, id);

	//Now add each word in the URL to map.

	string word;
	wb.getFirstWord(word, count);
	vector<IdCount>* existing = NULL;

	existing = m_wordToIdCounts.find(word);

	//If there is no Node for the current word.
	if(existing == NULL) //Create a new Node with word and vector to hold URL ID and count.
	{
		vector<IdCount> newVector;
		IdCount y(id, count);

		newVector.push_back(y);
		m_wordToIdCounts.associate(word, newVector);
	}
	else //Add the URL 
	{
		//Each word has at most one occurance of each ID and count for a given word will only be given once per URL, so no need to check for duplicates.
		IdCount y(id, count);
		existing->push_back(y);
	}


	while(wb.getNextWord(word, count) != false)
	{
		existing = m_wordToIdCounts.find(word);

		if (existing == NULL)
		{
			vector<IdCount> newVector;
			IdCount y(id, count);

			newVector.push_back(y);
			m_wordToIdCounts.associate(word, newVector);
		}
		else
		{
			IdCount y(id, count);
			existing->push_back(y);
		}
	}

	return true;
}

vector<UrlCount> IndexerImpl::getUrlCounts(string word)
{
	vector<UrlCount> urlVector;
	vector<IdCount>* existing = NULL;

	strToLower(word); //Convert word to lowercase.

	existing = m_wordToIdCounts.find(word);

	//If word isn't found, return empty vector of UrlCounts.
	if (existing == NULL)
		return urlVector;

	//Loop through word's vector of IdCounts, convert it to UrlCount, and push to Url Vector.
	for(int i = 0; i < existing->size(); i++)
	{
		IdCount tempIdCount;
		UrlCount tempUrlCount;

		tempIdCount = (*existing)[i];

		string * urlName;
		urlName = m_idToUrl.find(tempIdCount.id);

		if(urlName == NULL)
			break;

		tempUrlCount.url = *urlName;
		tempUrlCount.count = tempIdCount.count;

		urlVector.push_back(tempUrlCount);
	}

	return urlVector;  
}

template<typename KeyType, typename ValueType>
bool saveMyMap(string filename, MyMap<KeyType, ValueType>& m)
{
	ofstream outfile(filename.c_str());
	if (!outfile)
	{
		cerr << "ERROR: Cannot create file with filenameBase: " << filename << endl;
		return false;
	}
	KeyType key;
	ValueType * valueptr; 

	valueptr = m.getFirst(key);

	outfile << key << "\\" << *valueptr << endl;

	while ( valueptr != NULL)
	{
		valueptr = m.getNext(key);
		if (valueptr != NULL)
			outfile << key << "\\" << *valueptr << endl;
	}

	return true;
}

//For case where MyMap includes a vector.
bool saveMyMap(string filename, MyMap<string, vector<IdCount>>& m)
{
	ofstream outfile(filename.c_str());
	if (!outfile)
	{
		cerr << "ERROR: Cannot create file with filenameBase: " << filename << endl;
		return false;
	}

	string key;
	vector<IdCount> * valueptr; 

	valueptr = m.getFirst(key);

	
	if (valueptr != NULL)
	{
		outfile << key << "\\" ;
		
		for(int i = 0; i < valueptr->size(); i++)
		{
			outfile << (*valueptr)[i].id << "\\" << (*valueptr)[i].count << "\\" ;
		}

		outfile << endl;
	}

		valueptr = m.getNext(key);

		for ( ;valueptr != NULL; valueptr = m.getNext(key))
		{
			outfile << key << "\\" ;

				for(int i = 0; i < valueptr->size(); i++)
				{
					outfile << (*valueptr)[i].id << "\\" << (*valueptr)[i].count << "\\" ;
				}

			outfile << endl;
		}

		return true;
	
}

bool IndexerImpl::save(string filenameBase)
{
	return saveMyMap(filenameBase + ".u2i.txt", m_urlToId)  && 
           saveMyMap(filenameBase + ".i2u.txt", m_idToUrl) &&
           saveMyMap(filenameBase + ".w2ic.txt", m_wordToIdCounts);  
}


bool loadUrltoIdMap(string filename, MyMap<string, int>& m)
{
	ifstream infile(filename.c_str());
	if(!infile)
	{
		cerr << "ERROR: Cannot load: " << filename << endl;
		return false;
	}

	m.clear();

	string line;
	string url;
	int id;
	string idHold;

	while(getline(infile, line))
	{
		url = "";
		idHold = "";
		int position = 0;
		for(position; line[position] != '\\'; position++)
		{
			url += line[position];
		}
		position++;

		for(position; position != line.size(); position++)
		{
			idHold += line[position];
		}
		
		id = atoi(idHold.c_str());

		m.associate(url, id);
	}

	return true;
}

bool loadIdToUrlMap(string filename, MyMap<int, string>& m)
{
	ifstream infile(filename.c_str());

	if(!infile)
	{
		cerr << "ERROR: Cannot load: " << filename << endl;
		return false;
	}

	m.clear();

	string line;
	string url;
	int id;
	string idHold;

	while(getline(infile, line))
	{
		url = "";
		idHold = "";
		int position = 0;
		for(position; line[position] != '\\'; position++)
		{
			
			idHold += line[position];
		}
		position++;

		for(position; position != line.size(); position++)
		{
			url += line[position];
		}
		
		id = atoi(idHold.c_str());

		m.associate(id, url);
	}
	
	return true;
}

bool loadWordToIdCountsMap(string filename, MyMap<string, vector<IdCount>>& m)
{
	ifstream infile(filename.c_str());

	if(!infile)
	{
		cerr << "ERROR: Cannot load: " << filename << endl;
		return false;
	}

	m.clear();

	string line;
	string word;
	string urlIdHold;
	int urlId;
	string countHold;
	int count;

	while(getline(infile, line))
	{
		word = "";
		int position = 0;
		for(position; line[position] != '\\'; position++)
		{
			word += line[position];
		}
		position++;

		vector<IdCount> nodeVector;

		while(position != line.size())
		{
			countHold = "";
			urlIdHold = "";

			for(position; line[position] != '\\' && position != line.size(); position++)
			{
				urlIdHold += line[position];
			}
			if(position == line.size())
				break;
			else
				position++;
			
			urlId = atoi(urlIdHold.c_str());

			for(position; line[position] != '\\' && position != line.size(); position++)
			{
				countHold += line[position];
			}
			if(position == line.size())
				break;
			else
				position++;
			count = atoi(countHold.c_str());

			IdCount y(urlId, count);
			nodeVector.push_back(y);

		}

		m.associate(word, nodeVector);
	}

	return true;
}

bool IndexerImpl::load(string filenameBase)
{

	return loadUrltoIdMap(filenameBase + ".u2i.txt", m_urlToId)  && 
           loadIdToUrlMap(filenameBase + ".i2u.txt", m_idToUrl) &&
           loadWordToIdCountsMap(filenameBase + ".w2ic.txt", m_wordToIdCounts);  
}

//******************** Indexer functions *******************************

// These functions simply delegate to IndexerImpl's functions.
// You probably don't want to change any of this code.

Indexer::Indexer()
{
	m_impl = new IndexerImpl;
}

Indexer::~Indexer()
{
	delete m_impl;
}

bool Indexer::incorporate(string url, WordBag& wb)
{
	return m_impl->incorporate(url, wb);
}

vector<UrlCount> Indexer::getUrlCounts(string word)
{
	return m_impl->getUrlCounts(word);
}

bool Indexer::save(string filenameBase)
{
	return m_impl->save(filenameBase);
}

bool Indexer::load(string filenameBase)
{
	return m_impl->load(filenameBase);
}
