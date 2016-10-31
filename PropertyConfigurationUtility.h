#ifndef __PROPERTYCONFIGURATIONUTILITY_H__
#define __PROPERTYCONFIGURATIONUTILITY_H__

#include <map>
#include "ConfigurationUtility.h"

using namespace std;

typedef map<string, string> PropertyMap;
typedef map<string, string>::iterator PropertyMapIter;

//*******************************************************************
// This class provides and implementation of the configuration utility
// that works off a file with the format of a Java properties file
// this file takes the form of key=value. The key should not contain
// spaces (though, in this implementation it can it isn't a good idea)
// and the value is only read until the end of the line 
// any given entry can only contain a maximum of 255 characters (though
// this is configurable by the LINE_SIZE constant)
class PropertyConfigurationUtility : public ConfigurationUtility
{
public:
	~PropertyConfigurationUtility(void);

	virtual string get(const string&  key) throw (ConfigurationException);
	virtual StringVector getKeys(void);

protected:
	virtual void load(const string&  configFile) throw (ConfigurationException);

	const static long LINE_SIZE = 255;
	PropertyMap properties;
};

#endif

