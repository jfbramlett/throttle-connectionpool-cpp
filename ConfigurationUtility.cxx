#include "ConfigurationUtility.h"

//-------------------------------------------------------------------
// Constructor
ConfigurationUtility::ConfigurationUtility(void)
{
	m_initialized = false;
}

//-------------------------------------------------------------------
// Destructor - clean-up the class
ConfigurationUtility::~ConfigurationUtility(void)
{
}

//-------------------------------------------------------------------
// Loads the information in to the configuration utility
void ConfigurationUtility::loadFromFile(const string& fileName) throw (ConfigurationException)
{
	load(fileName);
	m_initialized = true;
}

//-------------------------------------------------------------------
// Returns a flag indicating if the configuration has been initialized
bool ConfigurationUtility::initialized() 
{
	return m_initialized;
}

