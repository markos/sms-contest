####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = evalserver

## INCLUDES were found outside kdevelop specific part

evalserver_SOURCES = strtokenizer.cpp sqliteconnection.cpp sqltable.cpp thread_pool.cpp client_handler.cpp client_acceptor.cpp processor_mo.cpp processor.cpp logmsg_mo.cpp logger.cpp day.cpp crc_32.cpp connectionmsgblock.cpp giftdetails.cpp contest.cpp timestamp.cpp timeperiod.cpp partdetails.cpp main.cpp 
evalserver_LDADD   = -lACE -lsqlite -lm

SUBDIRS = docs 

EXTRA_DIST = main.cpp partdetails.cpp partdetails.h timeperiod.cpp timeperiod.h timestamp.cpp timestamp.h contest.cpp contest.h giftdetails.cpp giftdetails.h connectionmsgblock.cpp connectionmsgblock.h crc_32.cpp crc_32.h day.cpp day.h Count.h logger.cpp logger.h logmsg_mo.cpp logmsg_mo.h processor.cpp processor.h processor_mo.cpp processor_mo.h client_acceptor.cpp client_acceptor.h client_handler.cpp client_handler.h thread_pool.cpp thread_pool.h sqliteconnection.h strtokenizer.h sqltable.cpp sqltable.h sqliteconnection.cpp strtokenizer.cpp reconstructdb 

####### kdevelop will overwrite this part!!! (end)############
# set the include path found by configure
INCLUDES= $(all_includes)
 
# the library search path.
evalserver_LDFLAGS = $(all_libraries) 
                                                                                
