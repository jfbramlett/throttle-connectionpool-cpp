#ifndef __DBCONNECTION_H__
#define __DBCONNECTION_H__

class DBConnection;

//*******************************************************************
// Class used to encapsulate a connection to the database. It is primarily
// an abstract interface that is used to encapsulate the necessary components
// for any type of database connection. It should be extended (with corresponding
// factory changes) for the different databases as required
class DBConnection 
{
public:
    DBConnection();
    virtual ~DBConnection();

    virtual bool testConnection();
    void invalidConnection(bool flag);
    bool invalidConnection();
    
private:
    bool m_invalidConnection;
};

#endif

