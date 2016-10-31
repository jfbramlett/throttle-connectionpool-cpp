#ifndef __CONFIGURATIONUTILITY_H__
#define __CONFIGURATIONUTILITY_H__

#include <string>
#include <vector>

using namespace std;

typedef vector<string> StringVector;
typedef vector<string>::iterator StringVectorIter;

//*******************************************************************
// Defines the exception class used to indicate an error has occured
// within the configuration service
class ConfigurationException
{
  public:
    ConfigurationException(const string& theMessage) : message(theMessage) {};
	string message;
};

//*******************************************************************
// Abstract base class used to define some form of file-based configuration
// that would use a key-value lookup mechanism
class ConfigurationUtility
{
public:
	ConfigurationUtility(void);
	virtual ~ConfigurationUtility(void);

	virtual string get(const string& key) throw (ConfigurationException) = 0;
	virtual StringVector getKeys(void) = 0;

	void loadFromFile(const string& fileName) throw (ConfigurationException);
	bool initialized();

protected:
	virtual void load(const string& fileName) throw (ConfigurationException) = 0;

	bool m_initialized;
};

#endif

