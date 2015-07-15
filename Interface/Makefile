# --------------------------------------------------------
# 	makefile for SQLiteIceInterface
# 	by hlding@sina.com
#   2015-05-29
# --------------------------------------------------------

INCLUDE := ../IceDef/IceInterface.h \
             SQLiteIceInterface.h   
            
SRC     := ../IceDef/IceInterface.cpp \
             SQLiteIceInterface.cpp  

CFLAGS  :=   -I./	 \
             -I../IceDef \
			 -I/usr/include/ice/include  \
			 -L/usr/lib            
                
all:libSQLiteIceInterface.so 

libSQLiteIceInterface.so : ${SRC} ${INCLUDE}
	g++ -o $@ ${CFLAGS} $^  -shared -g  -lIce -lIceUtil  -pthread -lpthread -lsqlite3
	
clean:
	rm libSQLiteIceInterface.so 

install:
	cp libSQLiteIceInterface.so  /usr/lib/libSQLiteIceInterface.so

uninstall:
	rm -rf ./libSQLiteIceInterface.so