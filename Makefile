#Makefile

CXX=gcc 
CFLAGS=-g -c
LFLAGS=-g -o 
AUXLIBS=-lm -pthread

TARGET=lo41
SRCFILE=main.c functions.c
OBJFILE=$(SRCFILE:.c=.o)

all : $(TARGET) 

restart: clean $(TARGET) run

$(TARGET): $(OBJFILE)
	@echo "***********************************"
	@echo "> Creating $@"
	@$(CXX) $^ $(LFLAGS) $@ $(LIBS) $(AUXLIBS)
	@echo ""
				
.cpp.o:
	@reset
	@echo "***********************************"
	@echo "> Generation of $@ from $<"
	@$(CXX) $(CFLAGS) $(INCLUDEDIR) -o $@ $<
	@echo ""

run:
	@echo "***********************************"
	@echo "> Launching $(TARGET)"
	@reset && export LD_LIBRARY_PATH=$(LIBSDIR) && ./$(TARGET)

clean:
	@echo "***********************************"
	@echo "> Cleaning directories"
	@find ./ -type f -name "*.o" -exec rm -rf {} \;
	@find ./ -type f -name "*.swp" -exec rm -rf {} \;
	@find ./ -type f -name "*~" -exec rm -rf {} \;
	@rm ./$(TARGET)
