CC = g++
LD = g++

CFLAGS = -Wall -pedantic-errors -Werror -std=c++11

BUILDDIR = build
OBJDIR = obj
SHARED_OBJS = $(addprefix $(OBJDIR)/,device.o receiver.o transmitter.o timescale.o)
OBJS_CentralHeating = $(addprefix $(OBJDIR)/, ch_main.o centralheating.o)
OBJS_Blinds = $(addprefix $(OBJDIR)/, b_main.o blinds.o)
HEADERS_CH = centralheating.h device.h
HEADERS_B = blinds.h device.h
HEADERS_SHARED = device.h receiver.h transmitter.h message.h timescale.h deviceinfo.h
SHARED_LIBS = -lcommoncpp -lccrtp -lpthread -lucommon

vpath %.cpp CentralHeating Blinds shared_src
vpath %.h CentralHeating  Blinds shared_src

all: CentralHeating Blinds

Blinds: $(BUILDDIR)/Blinds

CentralHeating: $(BUILDDIR)/CentralHeating

$(BUILDDIR)/Blinds: $(OBJS_Blinds) $(SHARED_OBJS) | $(BUILDDIR)
	$(LD) $^ -o $@ $(SHARED_LIBS)

$(BUILDDIR)/CentralHeating: $(OBJS_CentralHeating) $(SHARED_OBJS) | $(BUILDDIR)
	$(LD) $^ -o $@ $(SHARED_LIBS)

$(OBJS_CentralHeating): $(OBJDIR)/%.o: %.cpp $(HEADERS_CH) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS_Blinds): $(OBJDIR)/%.o: %.cpp $(HEADERS_B) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(SHARED_OBJS): $(OBJDIR)/%.o: %.cpp $(HEADERS_SHARED) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -rf build
	rm -rf obj
	rm -rf build-MainController-Desktop_Qt_5_11_1_GCC_64bit-Release
