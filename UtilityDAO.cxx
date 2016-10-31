//
//	Copyright (c) 2002 IONA Technologies PLC. All Rights Reserved.
//

#ifdef WIN32
#include "windows.h"
#else
#include "it_aspen/aspen.h"
#endif

#include "UtilityDAO.h"

#include <time.h>
#include <math.h>
#include <sys/timeb.h>

using namespace oracle::occi;
using namespace POM;


UtilityDAO::UtilityDAO()
{
	mMaxMngNo = 99;  //NUMBER(2) can't go higher than 99

   try {
      mMaxMngNo = calcMngNo();
   } catch (...) {
      //TRACELOG("critical error on UtilityDAO constructor");
      mMaxMngNo = 0;
   }

   // reset the counter on the mngNo
   mCounterMngNo = 0;

}



long UtilityDAO::calcMngNo()
{
   try {
      // initialize mngNo and spfNo
      Connection* lCon = DBPool::getConnection();

      Statement* lstmt;
      lstmt = lCon->createStatement("                               \
                 DECLARE                                            \
                    PROCEDURE count_mng (mng_no OUT INT)            \
                    IS                                              \
                    BEGIN                                           \
                       SELECT COUNT(*) INTO mng_no FROM MNG_PARAM;  \
                    END;                                            \
                 BEGIN                                              \
		    count_mng(:1);                                  \
                 END;");         

      lstmt->registerOutParam(1, OCCIINT, sizeof(int));
      lstmt->executeQuery();

      // substract 1 from count because there is a daddy manager
      // and save it
      mMaxMngNo = lstmt->getInt(1) - 1;

      // clean up the statement
      lCon->terminateStatement(lstmt);

   } catch (DBException dbex) {
	// should exit the program because need mngNo for processing
   } catch (SQLException sqlex) {
        // should exit the program because need mngNo for processing
   } catch (...) {
	// should exit the program because need mngNo for processing
   }

   return mMaxMngNo;

}


long UtilityDAO::calcPartitionID()
{
#ifndef WIN32
   long days;
   long partitionid;
   struct timeval sTp;
   struct tm sTm;
   gettimeofday(&sTp, NULL );
   localtime_r ( &(sTp.tv_sec), &sTm);
   days = (sTp.tv_sec/NUM_SEC_IN_DAY);
   partitionid = ((long)(days/NUM_DAYS_IN_PART) % NUM_PARTS) + 1;
   return partitionid;
#else
   return 4;
#endif
}

Number UtilityDAO::calcSrvTrxNo(char* srvTrxHold)
{
#ifndef WIN32
   char timestring[20];
   time_t cur_time;
   char *pos;
   long micro;

   struct timeval new_tm;
   struct timezone new_tz;
   struct tm sTm;

   gettimeofday(&new_tm, &new_tz);
   localtime_r ( &(new_tm.tv_sec), &sTm);
   cur_time = new_tm.tv_sec;

   strftime(timestring,20,"%y%j%H%M%S", &sTm);
   pos = timestring +1 ;
   micro = new_tm.tv_usec;
   sprintf(srvTrxHold,"%s%06d01", pos, micro);
#else
   sprintf(srvTrxHold,"%d18", GetTickCount());
#endif

   Number srvTrxNo(0);
   string tmp = (const char*)srvTrxHold;
   srvTrxNo.fromText(DBPool::getEnv(),tmp,"999999999999999999");

   if (srvTrxNo.isNull()) {
      //TRACELOG("number is null");
   }
   return srvTrxNo;
}

// use this function to get the next MngNo.
long UtilityDAO::getMngNo()
{
   try {
      // get the counter lock
      IT_Locker<IT_Mutex> locker(mMngLock);

      if (mCounterMngNo == mMaxMngNo) {
         mCounterMngNo = 1;
      } else {
         mCounterMngNo++;
      }

   } catch (...) {

      //TRACELOG("critical error in getting the next mgnNo");
   }

   // return the new count
   return mCounterMngNo;
}



Statement* UtilityDAO::getStatement(Connection* theCon, Statement*& pStatement, const char* szSQL)
{
	if (pStatement == NULL)
		pStatement = theCon->createStatement(szSQL);

	return pStatement;
}
