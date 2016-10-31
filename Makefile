CC=aCC

DAO_MODE_FLAG=-DUSE_ORACLE_DAO

INCDIR=$(TUXDIR)/include 

TUX_CFLAGS= -DNOWHAT=1 -I$(INCDIR)  -g1

TUX_LINK_CFLAGS=+DA1.1 -Wl,+s -D__BIGMSGQUEUE_ENABLED  -I$(TUXDIR)/include -o aspen_p2k_gw tux_main.cxx  -L$(TUXDIR)/lib aspen_p2k_gw.o  -ltux -lbuft   -lfml -lfml32 -lengine -lpthread -lcl -lnsl $(IT_LIBS)


# TUX_CFLAGS= -DNOWHAT=1 -I$(INCDIR)  -g1 


IT_CFLAGS=+objdebug -DITL_DEBUG_FULL -AA +W67 +W612 +We652,829 +p +DAportable -mt  -DIT_ASPEN_EXCLUDE_SSL -DHPUX -I$(ASPEN_HOME)/aspen/1.0/include   $(CGFLAG) $(DFML32) -I/opt/Sprint/products/oracle/demo -I/opt/Sprint/products/oracle/public -I/opt/Sprint/products/oracle/network/public $(DAO_MODE_FLAG)


#IT_CFLAGS=+objdebug -D__NO_DB__ -DITL_DEBUG_FULL -AA +W67 +W612 +We652,829 +p +DAportable -mt  -DIT_ASPEN_EXCLUDE_SSL -DHPUX -I/apps1/iona/aspen/1.0/include  -DIT_USE_ART_TS_ERROR_HANDLER  $(CGFLAG) $(DFML32) -I/apps1/iona/xerces-c1_4_0/include


IT_LIBS=\
 -L$(ASPEN_HOME)/aspen/1.0/lib \
 -L$(ASPEN_HOME)/shlib \
 -L$(ASPEN_HOME)/shlib/default \
 -L$(ASPEN_HOME)/xerces-c1_4_0/lib \
 -L$(ORACLE_HOME)/lib32 \
 -lit_art -lit_aspen -lit_ifc -lit_tuxedo -locci -lclntsh \
 -Wl,+s,+b,$(ASPEN_HOME)/shlib:$(ASPEN_HOME)/shlib/default  \
 -Wl,-E -Wl,-Bdeferred -Wl,-Bnonfatal -Wl,-Bverbose -Wl,-Bsymbolic


TUX_LIB_FLAGS=-L$(TUXDIR)/lib  +DA1.1 -Wl,+s  -D__BIGMSGQUEUE_ENABLED \
    -ltux -lbuft  -lfml -lfml32 -lengine  -lpthread -lcl  -lnsl 



.cxx.o:
	$(CC) $(TUX_CFLAGS) $(IT_CFLAGS) -c $*.cxx



CLIENT_OBJS=  tuxclient.o


SERVER_OBJS= ConfigurationUtility.o \
    DBConnection.o \
    DBConnectionFactory.o \
    DBConnectionPtr.o \
    DBPool.o \
    DBPoolConfiguration.o \
    DBPoolTestCase.o \
    Main.o \
    OracleConnection.o \
    P2KGatewayConfiguration.o \
    POMThrottleAdapter.o \
    PropertyConfigurationUtility.o \
    TestCase.o \
    Throttle.o \
    ThrottleAdapter.o \
    ThrottleAdapter.o \
    ThrottleAdapterFactory.o 

#    Exception.o HTTPClient.o HTTPCommon.o HTTP_c.o \
#    MIMEEncoder.o StreamHandler.o StringCollection.o \
#    VariantVector.o WebClient.o XMLHelper.o aspen.o \
#    utstrlst.o uttaglst.o StreamHandler.o 

all: tuxserver

tuxclient : $(CLIENT_OBJS) 
	$(CC) -o tuxclient $(CLIENT_OBJS) $(IT_CFLAGS) $(TUX_LIB_FLAGS) $(IT_LIBS)


tuxserver : $(SERVER_OBJS)
	aCC $(IT_CFLAGS) +DA1.1 -Wl,+s -D__BIGMSGQUEUE_ENABLED  -I$(TUXDIR)/include -o p2k_gw $(SERVER_OBJS) -L$(TUXDIR)/lib -ltux -lbuft   -lfml -lfml32 -lengine -lpthread -lcl -lnsl $(IT_LIBS)


clean: 
	rm -f aspen_p2k_gw
	rm -f tuxclient
	rm -f *.o

#	buildserver -v -k -t -o aspen_p2k_gw -f aspen_p2k_gw.o -l "/vob/stage_o2k_HP-UX_debug/asp/6.0/lib/libit_art.sl /vob/stage_o2k_HP-UX_debug/asp/6.0/lib/libit_ifc.sl" -s p2k_gw







#/vob/stage_o2k_HP-UX_debug/asp/6.0/lib/libit_art.sl /vob/stage_o2k_HP-UX_debug/asp/6.0/lib/libit_ifc.sl 

