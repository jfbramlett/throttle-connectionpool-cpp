#include "PropertyConfigurationUtility.h"
#include <fstream>

using namespace std ;

//-------------------------------------------------------------------
// Destructor - clean-up the class
PropertyConfigurationUtility::~PropertyConfigurationUtility(void)
{
}

//-------------------------------------------------------------------
// Gets the value in the config with the given key. If the key does not
// exist in the configuration a ConfigurationException is thrown
string PropertyConfigurationUtility::get(const string&  key) throw (ConfigurationException)
{
	string value;
	PropertyMapIter iter;

	// look-up the given key in the property collection
	iter = properties.find(key);
	
	// if we found them item we're golden
	if (iter != properties.end())
	{
		value = iter->second;
	}
	else {
		// we did not find out value in our hashmap so throw an exception
        // to indicate we don't have this key
        throw ConfigurationException("Failed to find key " + key + " in configuration!");
	}

	return value;
}

//-------------------------------------------------------------------
// Gets the list of keys from the config
StringVector PropertyConfigurationUtility::getKeys(void)
{
	StringVector keys;
	
	// iterate our property collection to pull out the keys
	PropertyMapIter iter = properties.begin();
	while (iter != properties.end()) 
	{
		keys.push_back(iter->first);
	}
	
	return keys;
}

//-------------------------------------------------------------------
// Reads the configuration file loading our property collection
void PropertyConfigurationUtility::load(const string&  configFile) throw (ConfigurationException)
{
	// get the sleep value from our configuration file
	ifstream file(configFile.c_str());
	if (!file) 
	{
		// bad news we could not open our file
		throw ConfigurationException("Failed to open configuration file " + configFile + " - unable to initialize configuration service!");
	}

	// now read in the file
	char line[LINE_SIZE];
	while (!file.eof()) 
	{
		// read the line from the file
		file.getline(line, LINE_SIZE);

		// now parse this line to get our key/value and add them to our
		// property collection
		string strLine = line;
		size_t pos = strLine.find('=');
		if (pos != string::npos) 
		{
			string key = strLine.substr(0, pos);
			string value = strLine.substr(pos+1, strLine.length() - key.length());
			properties[key] = value;
		}
	}
}
