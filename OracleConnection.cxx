#include "OracleConnection.h"
#include "DBException.h"

//-------------------------------------------------------------------
// Constructor
OracleConnection::OracleConnection(const string& userid, const string& password, const string& connectString)
      : m_environment(NULL), m_connection(NULL)
{
   getConnection(userid, password, connectString);
}

//-------------------------------------------------------------------
// Destructor
OracleConnection::~OracleConnection()
{
   releaseConnection();
}

//-------------------------------------------------------------------
// Gets the underlying oracle connection (occi connection)
Connection* OracleConnection::getConnection()
{
    // return our database connection - the connection is obtained when
    // this object is constructed
#ifndef __NO_DB__
    return m_connection;
#else
    return NULL;
#endif
}

//-------------------------------------------------------------------
// Gets a new connection to the database 
void OracleConnection::getConnection(const string& userid, const string& password, const string& connectString)
{
#ifndef __NO_DB__
    // initialize our environment
    m_environment = Environment::createEnvironment (Environment::DEFAULT);

    try 
    {
        // get a new connection from the oracle environment
	m_connection = m_environment->createConnection(userid, password, connectString);
    } 
   catch (SQLException ex) 
   {
       throw DBException((string)ex.getMessage().c_str());
   } 
   catch (...)  
   {
       throw DBException("An unexpected error occurred trying to get a connection to the database!");
   }   
#endif
}

//-------------------------------------------------------------------
// Releases the resources associated with the connection
void OracleConnection::releaseConnection()
{
#ifndef __NO_DB__
   try 
   {
        // need to move to explictit cleanup
	m_environment->terminateConnection(m_connection);
	Environment::terminateEnvironment(m_environment);
   } 
   catch (SQLException sqlex) 
   {
       throw DBException((string)sqlex.getMessage().c_str());
   }
#endif
}
