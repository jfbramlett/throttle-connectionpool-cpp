//
//	Copyright (c) 2002 IONA Technologies PLC. All Rights Reserved.
//

#ifndef _UTILITYDAO_H
#define _UTILITYDAO_H

#include "DBPool.h"
#include <it_ts/mutex.h>
#include <it_ts/locker.h>

namespace POM {

const long NUM_SEC_IN_DAY = 86400;
const long NUM_PARTS = 10;
const long NUM_DAYS_IN_PART = 4;


class UtilityDAO {

private:
	long mMaxMngNo;
   
	// counter for MngNo
	long mCounterMngNo;
	// lock for the counter - USE IT!
    IT_Mutex mMngLock;
	 

public:

	UtilityDAO();

	// calculates the maximum MngNo
	long calcMngNo();
	// use this function to get the next MngNo.
	long getMngNo();


	long calcPartitionID();

	Number calcSrvTrxNo(char*);

	Statement* getStatement(Connection* theCon, Statement*& pStatement, const char* szSQL);
};


};


#endif

