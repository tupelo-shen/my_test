SIM_OBJS = main.o
SRC_OBJS = src/test.o \
			src/Average.o

SIMULATOR = simulator.exe
# -----<< Tool chaine >>-----
CMNINC	= -I inc/ -I src/
GCC		= gcc
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
LIBS =


ARMCC	= armcc
ARMLINK	= armlink
FROMELF	= fromelf
WINE_INC = -I/c/Keil_v5/ARM/Pack/Keil/Kinetis_K60_DFP/1.4.0/Device/Include \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS_RTX/INC \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Include \
	   -I/c/Keil_v5/ARM/Pack/ARM/CMSIS/4.2.0/CMSIS/Driver/Include
endif

all: sim
	@bash --norc -c "date '+%Y%m%d %H:%M %S'"
	@echo finished of making Extutable file.
	
sim: $(SIMULATOR) Makefile-gcc.dep
	@echo $(SIMULATOR) build finished.

Makefile-gcc.dep: src/*.cpp Makefile
	@g++ -MM $(CMNINC) src/*.cpp    | sed -e 's/^\([^ ]\)/src\/\1/' > $@
	
$(SIMULATOR): main.cpp src/*.h $(SRC_OBJS)
	@echo testing...
	$(GPP) -o $(SIMULATOR) main.cpp $(SRC_OBJS) $(GPPFLAGS) $(LIBS)	

src/%.o : src/%.c
	$(GPP) -c           $(GPPFLAGS) -o $@ $<

depend: Makefile-gcc.dep
	ls -l *.dep
clean:
	rm -f $(SRC_OBJS) *.o *.exe 