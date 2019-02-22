# -----<< All objects >>-----
SIM_OBJS = main.o
SRC_DIR = src
SRC_TEST_DIR = src/thread-test
SRC_OBJS = src/BindingTest.o \
			src/Average.o \
			src/IntArray.o \
			src/Common.o \
			src/AbstractProduct.o \
			src/SimpleFactory.o \
			src/AbstractFactory.o

GTEST_OBJS = 	gtest/gtest_Average.o \
				gtest/gtest_main.o

UNIT_TEST = $(GTEST_OBJS) $(SRC_OBJS)

SIMULATOR = simulator.exe
GTEST_SIMULATOR = gtest_simulator.exe

# -----<< Tool chaine >>-----
CMNINC	= -I inc/ -I src/ -I inc/googletest_msys32/include \
		  -I inc/ana_stl \
		  -I inc/gtest
# CMNINC	= -I inc/ -I src/ -I inc/googletest_msys32/include \
# 		  -I inc/gtest

GPP		= g++

GCCFLAGS = -g -Wall -pipe -m32 -finput-charset=UTF-8 -fexec-charset=UTF-8 \
			$(CMNINC) \
			`pkg-config --cflags --libs gtk+-3.0`
#			-D_WIN32_WINNT=0x0501
GPPFLAGS =  -std=c++0x $(GCCFLAGS) -DBOOST_THREAD_USE_LIB 

ifndef MINGW32_HOME
LIBS = 

ARMCC	= wine /home/30015139/.wine/drive_c/Keil_v5/ARM/ARMCC/bin/armcc
ARMLINK	= wine /home/30015139/.wine/drive_c/Keil_v5/ARM/ARMCC/bin/armlink
FROMELF	= wine /home/30015139/.wine/drive_c/Keil_v5/ARM/ARMCC/bin/fromelf
WINE_INC = -I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/Keil/Kinetis_K60_DFP/1.4.0/Device/Include \
	   -3-I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS_RTX/INC \
	   -I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Include \
	   -I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Driver/Include
else
LIBS = 	-L libs/ \
		-L libs/googletest_msys32/lib/ \
			-lgtest-mt


ARMCC	= armcc
ARMLINK	= armlink
FROMELF	= fromelf
WINE_INC = -I/c/Keil_v5/ARM/Pack/Keil/Kinetis_K60_DFP/1.4.0/Device/Include \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS_RTX/INC \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Include \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Driver/Include
endif

all: sim gtest
	@bash --norc -c "date '+%Y-%m-%d %H:%M %S'"
	@echo finished of making Extutable file.
	
sim: $(SIMULATOR) Makefile-gcc.dep
	@echo $(SIMULATOR) build finished.

gtest: $(GTEST_SIMULATOR) Makefile-gtest.dep
	@echo $(GTEST_SIMULATOR) build finished.

Makefile-gcc.dep: src/*.cpp Makefile
	@g++ -MM $(CMNINC) src/*.cpp    | sed -e 's/^\([^ ]\)/src\/\1/' > $@

Makefile-gtest.dep:gtest/*.cpp Makefile	
	@g++ -MM $(CMNINC) gtest/*.cpp    | sed -e 's/^\([^ ]\)/gtest\/\1/' > $@ 

$(SIMULATOR): main.cpp src/*.h $(SRC_OBJS)
	@echo compiling...
	$(GPP) -o $(SIMULATOR) main.cpp $(SRC_OBJS) $(GPPFLAGS) $(LIBS)	

$(GTEST_SIMULATOR): $(UNIT_TEST)
	@echo testing...
	$(GPP) -o $(GTEST_SIMULATOR) $(UNIT_TEST) $(GPPFLAGS) $(LIBS)
	./$@

src/%.o : src/%.cpp
	$(GPP) -c           $(GPPFLAGS) -o $@ $<

gtest/%.o : gtest/%.cpp
	$(GPP) -c     		$(GPPFLAGS) -o $@ $< 

depend: Makefile-gcc.dep Makefile-gtest.dep
	ls -l *.dep

clean:
	rm -f *.o *.exe $(SRC_DIR)/*.o $(SRC_DIR)/*.exe $(SRC_TEST_DIR)/*.o $(SRC_TEST_DIR)/*.exe $(GTEST)/*.o 