####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = evalstat

## INCLUDES were found outside kdevelop specific part

evalstat_SOURCES = strtokenizer.cpp timeperiod.cpp timestamp.cpp sqliteconnection.cpp logger.cpp evaldata.cpp report.cpp htmleditor.cpp main.cpp 
evalstat_LDADD   = -lsqlite -lACE


EXTRA_DIST = main.cpp htmleditor.cpp htmleditor.h report.cpp report.h evaldata.cpp evaldata.h sqltable.h logger.cpp logger.h sqliteconnection.cpp sqliteconnection.h timestamp.cpp timestamp.h timeperiod.cpp timeperiod.h strtokenizer.cpp strtokenizer.h 

####### kdevelop will overwrite this part!!! (end)############
# set the include path found by configure
INCLUDES= $(all_includes)
 
# the library search path.
evalstat_LDFLAGS = $(all_libraries) 
                                                                                
