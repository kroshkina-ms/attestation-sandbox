TARGET := jwt-verifier

CXX := g++
CXXFLAGS := -g -Wall -std=c++17 -pthread

ROOTDIR := .
OBJDIR := $(ROOTDIR)/obj
INCDIR := $(ROOTDIR)/include
SRCDIR := $(ROOTDIR)/src
VCPKGDIR := $(HOME)/attestation-sandbox/vcpkg/packages
INCS := $(wildcard *.hpp $(foreach fd, $(INCDIR), $(fd)/*.hpp))
SRCS := $(wildcard *.cpp $(foreach fd, $(SRCDIR), $(fd)/*.cpp))
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

INCLUDES := -I. -I$(INCDIR) -I$(shell pkg-config oehostverify-$(CXX) --variable=includedir) $(shell pkg-config oehostverify-$(CXX) --cflags)
LIBINCLUDES := -L/usr/lib/ssl -L/usr/lib
INC := $(INCLUDES) $(LIBINCLUDES)
LIBS := -lssl -lcrypto -lcurl $(shell pkg-config oehostverify-$(CXX) --libs)

# rules

.PHONY: dump clean distclean

$(TARGET): $(OBJDIR) $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INC) -o $(TARGET) $(OBJS) $(LIBS) 

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCS)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $< $(LIBS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

dump:
	@echo "HOME     : $(HOME)"
	@echo "ROOTDIR  : $(ROOTDIR)"
	@echo "OBJDIR   : $(OBJDIR)"
	@echo "INCDIR   : $(INCDIR)"
	@echo "SRCDIR   : $(SRCDIR)"
	@echo "VCPKGDIR : $(VCPKGDIR)"
	@echo "INCS     : $(INCS)"
	@echo "SRCS     : $(SRCS)"
	@echo "OBJS     : $(OBJS)"
	@echo "INCLUDES : $(INCLUDES)"
	@echo "LIBS     : $(LIBS)"
	@echo "CXXFLAGS : $(CXXFLAGS)"

clean:
	@rm -vf *.o ./**/*.o
	@rm -rvf $(OBJDIR)

distclean: clean
	@rm -vf $(TARGET)

