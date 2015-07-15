# --------------------------------------------------------
# 	makefile for SQLiteServer
# 	by hlding@sina.com
#   2015-06-06
# --------------------------------------------------------

HEADFILES := ../IceDef/IceInterface.h \
            CppSQLite3.h    \
            SQLiteService.h
            
SRCFILES  := ../IceDef/IceInterface.cpp \
            CppSQLite3.cpp  \
            SQLiteService.cpp

CFLAGS   :=   -I./	\
             -I../IceDef \
			 -I/usr/include/ice/include  \
			 -L/usr/lib            

all:libSQLiteServer.so 

libSQLiteServer.so : ${SRCFILES} ${HEADFILES}
	g++ -o $@ ${CFLAGS} $^  -shared -g  -lIce -lIceUtil  -lsqlite3
	
clean:
	rm libSQLiteServer.so 

install:
	cp libSQLiteServer.so  /usr/lib/libSQLiteServer.so

uninstall:
	rm -rf ./libSQLiteServer.so