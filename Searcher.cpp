#include "provided.h"
#include <string>
#include "MyMap.h"

using namespace std;

class SearcherImpl
{
public:
    vector<string> search(string terms);
    bool load(string filenameBase);

private:
	Indexer m_index;

	struct UrlData
	{
		UrlData(){}
		UrlData(int normal, int unique) {matches = normal; uniqueCount = unique;}
		int matches;
		int uniqueCount;
	};
	
};

//true if left side comes before. Compares two at time by uniqueCount.
//If values are equal, compare by URL alphabetical order.
bool UrlDataCompare(const UrlCount& a, const UrlCount& b)
{
	if (a.count > b.count)
		return true;
	else if (a.count == b.count)
	{
		if (a.url >= b.url)
			return true;
		else 
			return false;
	}
	else
		return false;
}

vector<string> SearcherImpl::search(string terms)
{
	//Input search words into map and make them case insensitive.
	Tokenizer t(terms);
	string w;
	MyMap<string, int> termsMap;
	int N = 0;
	while(t.getNextToken(w))
	{
		strToLower(w);
		int * count = termsMap.find(w);

		if(count == NULL)
		{
			termsMap.associate(w, 1);
			N++; //Get number of unique terms.
		}
		else
		{
			termsMap.associate(w, *count+1);
		}
	}
	

	//Set T
	int T = 1;
	int relevance = 0;
	if(int(0.7*N) > 1)
		T = int(0.7*N);
	
	MyMap<int, string> matchesPerUrl; //Map that holds URLs and number of matches to a term it has.

	string word;
	termsMap.getFirst(word);

	//<KeyType, ValueType>
	MyMap<string, UrlData> matchMap;
	//Search for first word.
	vector<UrlCount> urlCountVector = m_index.getUrlCounts(word);

	//Put all URL matches into a map and keep track of their # of matches and their unique number of occurances for each search term.

	for(int i = 0; i < urlCountVector.size(); i++)
	{
			UrlData * check = NULL;
			string url = urlCountVector[i].url;
			int currentCount = 0;
			check = matchMap.find(url);
			currentCount = urlCountVector[i].count;
			int matches = 1;

			if (check == NULL)
			{
				UrlData data(1, currentCount);
				matchMap.associate(url, data);
			}
			else
			{
				check->matches++;
				check->uniqueCount += currentCount;
				matchMap.associate(url, *check);
			}
	}

	//Loop again for rest of terms.

	while (termsMap.getNext(word) != NULL)
	{
		vector<UrlCount> urlCountVector = m_index.getUrlCounts(word);

		for(int i = 0; i < urlCountVector.size(); i++)
		{
			UrlData * check = NULL;
			string url = urlCountVector[i].url;
			int currentCount = 0;
			check = matchMap.find(url);
			currentCount = urlCountVector[i].count;
			int matches = 1;

			if (check == NULL)
			{
				UrlData data(1, currentCount);
				matchMap.associate(url, data);
			}
			else
			{
				check->matches++;
				check->uniqueCount += currentCount;
				matchMap.associate(url, *check);
			}
		}
	}

	//Here I have matchMap which includes all URLs with matches.
	//Has the number of matches and the unique number of matches += every term.

	//Now, add URLs that qualify as a match into a vector before we sort them.'

	vector<UrlCount> matches; //Will hold all qualifying URLs.
	vector<string> orderedUrls; //Will hold the results.

	string nextUrl;

	UrlData * countQualify;
	UrlCount add; //Used to add to vector.

	countQualify = matchMap.getFirst(nextUrl);

	if (countQualify == NULL)
		return orderedUrls;

	if (countQualify->matches >= T)
	{
		add.url = nextUrl;
		add.count = countQualify->uniqueCount;
		matches.push_back(add);
	}

	while ((countQualify = matchMap.getNext(nextUrl)) != NULL)
	{
		if (countQualify->matches >= T)
		{
			add.url = nextUrl;
			add.count = countQualify->uniqueCount;
			matches.push_back(add);
		}
	}

	sort(matches.begin(), matches.end(), UrlDataCompare);

	//matches is now sorted.
	//Put URLs in order into the string vector of URLs.

	for (int i = 0; i < matches.size(); i++)
	{
		orderedUrls.push_back(matches[i].url);
	}

	//Now orderedUrls contains URLs in sorted order.

	return orderedUrls;
}

bool SearcherImpl::load(string filenameBase)
{
    return m_index.load(filenameBase);  // This compiles, but may not be correct
}

//******************** Searcher functions *******************************

// These functions simply delegate to SearcherImpl's functions.
// You probably don't want to change any of this code.

Searcher::Searcher()
{
    m_impl = new SearcherImpl;
}

Searcher::~Searcher()
{
    delete m_impl;
}

vector<string> Searcher::search(string terms)
{
    return m_impl->search(terms);
}

bool Searcher::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
