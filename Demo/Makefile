# Makefile
#
# ============================================================================
#    	hlding@wiscom.com.cn for test SQLiteIceInterface
#       2015-06-06
# ============================================================================

TARGET    = $(notdir $(CURDIR))

CFLAGS :=   -I./	\
            -I../Interface \
			-I/usr/include/ice/include  

LD_FLAGS += -L. \
            -L../Interface \
			-L/usr/lib            
			
COMPILE_C = g++ $(C_FLAGS) -c
LINK_C    = g++ $(LD_FLAGS)  
DBGTARGET = debug/$(TARGET)d
RELTARGET = release/$(TARGET)

DBGCFLAGS = -g -DDEBUG
RELCFLAGS = -O2 -fno-strict-aliasing

SOURCES = $(wildcard *.cpp) 
HEADERS = $(wildcard *.h)   

DBGOBJFILES = $(SOURCES:%.cpp=debug/%.o)
RELOBJFILES = $(SOURCES:%.cpp=release/%.o)

RELLDFLAGS =  
DBGLDFLAGS =  
	      

.PHONY: clean debug release install

all:	debug release

install:
	install $(DBGTARGET) ./
#	$(CODEGEN_PREFIX)strip $(RELTARGET)
    
release:	$(RELTARGET)

debug:		$(DBGTARGET)

$(RELTARGET):	$(RELOBJFILES) 
	$(LINK_C) -o $@ $^ $(RELLDFLAGS) -lpthread -lSQLiteIceInterface

$(DBGTARGET):	$(DBGOBJFILES) 
	$(LINK_C) -o $@ $^ $(DBGLDFLAGS) -lpthread -lSQLiteIceInterface

$(RELOBJFILES):	release/%.o: %.cpp $(HEADERS) 
	@mkdir -p release
	$(COMPILE_C) $(RELCFLAGS)  -o $@ $<

$(DBGOBJFILES):	debug/%.o: %.cpp $(HEADERS) 
	@mkdir -p debug
	$(COMPILE_C) $(DBGCFLAGS) -o $@ $<

clean:
	-$(RM) -rf release debug *~ *.d .dep
