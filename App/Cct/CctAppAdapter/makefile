LIB = App

#source dir


CCTINC = ../../../App/Cct
DBINC = ../../../App/DB
SQLINC = ../../../App/DB/SQLite
PROINC = ../../../App/Proto
PROIFINC = ../../../App/Proto/If
PROTOCOLINC = ../../../App/Proto/CctAppAdapter
MAININC = ../../../Prj/Linux/main
ARCHINC = ../../../Prj/Linux/sysarch

INCS = -I$(CCTINC) -I$(DBINC) -I$(SQLINC) -I$(PROINC) -I$(PROIFINC) -I$(PROTOCOLINC) -I$(MAININC)  -I$(ARCHINC)
		
LIBS = -lnsl -lpthread -ldl

##################################################
ROOTDIR = ../../../Prj/Linux
OBJDIR = $(ROOTDIR)/obj
BINDIR = $(ROOTDIR)/bin
LIBDIR = $(ROOTDIR)/lib

##################################################
SRCDIR = .
SRCS = $(notdir $(wildcard $(SRCDIR)/*.c $(SRCDIR)/*.cpp))
	
#OBJS = $(SRCS:%.=$(OBJDIR)/%.o)
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS)))

##################################################
#CC       =  g++
#AR       = ar
CC       = arm-buildroot-linux-uclibcgnueabi-g++
AR       = arm-buildroot-linux-uclibcgnueabi-ar
CPPFLAGS = -g -c -Os $(INCS)
CPPFLAGS += -DLINUX -fno-builtin-log
LDFLAGS  = -L$(LIBDIR) $(LIBS)
#########################################################
$(LIBDIR)/lib$(LIB).a: $(LIBDIR) $(OBJDIR) $(OBJS) 
	$(AR) rv $@ $(OBJS)
	ranlib $@

$(OBJDIR):
	@test -d $@ || mkdir $@
	
$(LIBDIR):
	@test -d $@ || mkdir $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CPPFLAGS) -o $@ $<
	
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	@rm -r -f $(OBJDIR) $(LIBDIR)/lib$(LIB).a $(SRCDIR)/*~