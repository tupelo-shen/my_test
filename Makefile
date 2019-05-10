# -----<< All objects >>-----
SIM_OBJS = main.o
SRC_DIR = src
STL_SRC_DIR = src/ana_stl
GTEST_DIR = gtest
SRC_TEST_DIR = src/thread-test
SRC_OBJS = src/BindingTest.o \
			src/Average.o \
			src/IntArray.o \
			src/Common.o \
			src/AbstractProduct.o \
			src/SimpleFactory.o \
			src/AbstractFactory.o \
			src/stl_math.o \
			src/PF_EEPROM_Driver.o \
			src/PF_UART_Driver.o \
			src/SIM_BlockingReader.o \
			src/SIM_UartTimerSim.o \
			src/SIM_UartSim.o \
			src/SIM_UartSimHart.o \
			src/SIM_Option.o \
			src/SIM_SerialPort.o \
			src/COM_CrcCalculation.o

# ANA_STL_OBJS = 	src/ana_stl/stl_math.o

GTEST_OBJS = 	gtest/gtest_Average.o \
				gtest/gtest_main.o

GMOCK_OBJS =	gtest/gmock_test.o \
				gtest/gmock_AccountHelper.o \
				gtest/gmock_AccountService.o \
				gtest/gmock_AccountTest.o

UNIT_TEST = $(GTEST_OBJS) $(SRC_OBJS)
UNIT_TEST1 = $(GMOCK_OBJS) $(SRC_OBJS)

SIMULATOR = simulator.exe
GTEST_SIMULATOR = gtest_simulator.exe
GMOCK_SIMULATOR = gmock_simulator.exe

# -----<< Tool chaine >>-----
CMNINC	= -I inc/ -I src/ -I libs/googletest_msys32/include \
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
LIBS = -L Libraries \
				-lboost_thread \
				-lboost_regex \
				-lboost_filesystem \
				-lboost_system \
				-lboost_program_options \
				-lboost_chrono \
				-lrt \
				-lana_mrb \
				-lana_stl \
				-lmruby \
				-lliq_hart \
				-lgtest 

ARMCC	= wine /home/30015139/.wine/drive_c/Keil_v5/ARM/ARMCC/bin/armcc
ARMLINK	= wine /home/30015139/.wine/drive_c/Keil_v5/ARM/ARMCC/bin/armlink
FROMELF	= wine /home/30015139/.wine/drive_c/Keil_v5/ARM/ARMCC/bin/fromelf
WINE_INC = -I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/Keil/Kinetis_K60_DFP/1.4.0/Device/Include \
	   -3-I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS_RTX/INC \
	   -I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Include \
	   -I/home/30015139/.wine/drive_c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Driver/Include
else
# LIBS = 	-L libs/ \
# 		-L libs/googletest_msys32/lib/ \
# 			-lgtest-mt
LIBS =	-L libs/googletest_msys32/lib/ \
			-lgtest \
			-lgmock \
			-lwsock32 \
			-lws2_32 \
			-lboost_chrono-mt \
			-lboost_thread-mt \
			-lboost_system-mt \
			-lboost_regex-mt \
			-lboost_filesystem-mt \
			-lboost_program_options-mt
			# -lmruby_mt \
			# -lana_stl-mt \
			# -lana_mrb-mt \
			# -lliq_hart_mt \
			
ARMCC	= armcc
ARMLINK	= armlink
FROMELF	= fromelf
WINE_INC = -I/c/Keil_v5/ARM/Pack/Keil/Kinetis_K60_DFP/1.4.0/Device/Include \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS_RTX/INC \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Include \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Driver/Include
endif

# all: sim gtest gmock
all: gtest gmock
	@bash --norc -c "date '+%Y-%m-%d %H:%M %S'"
	@echo finished of making Extutable file.
	
sim: $(SIMULATOR) Makefile-gcc.dep
	@echo $(SIMULATOR) build finished.

gtest: $(GTEST_SIMULATOR) Makefile-gtest.dep
	@echo $(GTEST_SIMULATOR) build finished.

gmock: $(GMOCK_SIMULATOR) Makefile-gmock.dep
	@echo $(GMOCK_SIMULATOR) build finished.

Makefile-gcc.dep: src/*.[ch]* src/ana_stl/*.c* inc/ana_stl/*.h* Makefile
	@g++ -MM $(CMNINC) src/*.c*    | sed -e 's/^\([^ ]\)/src\/\1/' > $@
	@g++ -MM $(CMNINC) $(STL_SRC_DIR)/*.c* | sed -e 's/^\([^ ]\)/src\/\1/' >> $@

Makefile-gtest.dep:gtest/*.cpp Makefile	
	@g++ -MM $(CMNINC) gtest/*.cpp    | sed -e 's/^\([^ ]\)/gtest\/\1/' > $@

Makefile-gmock.dep:gtest/*.cpp Makefile	
	@g++ -MM $(CMNINC) gtest/*.cpp    | sed -e 's/^\([^ ]\)/gtest\/\1/' > $@

$(SIMULATOR): main.cpp src/*.h inc/ana_stl/*.h $(SRC_OBJS)
	@echo compiling...
	$(GPP) -o $(SIMULATOR) main.cpp $(SRC_OBJS) $(GPPFLAGS) $(LIBS)	

$(GTEST_SIMULATOR): $(UNIT_TEST)
	@echo testing...
	$(GPP) -o $(GTEST_SIMULATOR) $(UNIT_TEST) $(GPPFLAGS) $(LIBS)
	@bash --norc -c "date '+%Y-%m-%d %H:%M %S'" > $(GTEST_SIMULATOR).txt
	./$@ >> $(GTEST_SIMULATOR).txt

$(GMOCK_SIMULATOR): $(UNIT_TEST1)
	@echo testing...
	$(GPP) -o $(GMOCK_SIMULATOR) $(UNIT_TEST1) $(GPPFLAGS) $(LIBS)
	@bash --norc -c "date '+%Y-%m-%d %H:%M %S'" > $(GMOCK_SIMULATOR).txt
	./$@ >> $(GMOCK_SIMULATOR).txt

gmock_macro : 
	$(GPP) -E -P 		gtest/gmock_test.cpp > gmock_test_macro_expansion.cpp $(CMNINC) $(LIBS)
	$(GPP) -E -P 		gtest/gtest_Average.cpp > gtest_Average_macro_expansion.cpp $(CMNINC) $(LIBS)
	@echo gmock macro expansion build finished.

$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(GPP) -c           $(GPPFLAGS) -o $@ $<

$(SRC_DIR)/%.o : $(STL_SRC_DIR)/%.cpp
	$(GPP) -c           $(GPPFLAGS) -o $@ $<

gtest/%.o : gtest/%.cpp
	$(GPP) -c     		$(GPPFLAGS) -o $@ $<

depend: Makefile-gcc.dep Makefile-gtest.dep Makefile-gmock.dep
	ls -l *.dep

clean:
	rm -f *.o *.exe $(SRC_DIR)/*.o $(SRC_DIR)/*.exe $(SRC_TEST_DIR)/*.o $(SRC_TEST_DIR)/*.exe \
	$(GTEST_DIR)/*.o  gtest_Average_macro_expansion.cpp  gmock_test_macro_expansion.cpp \
	$(STL_SRC_DIR)/*.o