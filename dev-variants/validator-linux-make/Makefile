TARGET := jwt-validator

CXX := g++
#CXXFLAGS := -v -g -Wall -std=c++17 -pthread
CXXFLAGS := -g -Wall -std=c++17 -pthread

ROOTDIR := .
OUTDIR := $(ROOTDIR)/out
OBJDIR := $(ROOTDIR)/obj
INCDIR := $(ROOTDIR)/include
SRCDIR := $(ROOTDIR)/src
VCPKGDIR := $(HOME)/attestation-sandbox/vcpkg/packages
INCS := $(wildcard *.hpp $(foreach fd, $(INCDIR), $(fd)/*.hpp))
SRCS := $(wildcard *.cpp $(foreach fd, $(SRCDIR), $(fd)/*.cpp))
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

#INCLUDES := -I. -I$(INCDIR) -I$(VCPKGDIR)/openssl_x64-linux/include -I$(VCPKGDIR)/curl_x64-linux/include
#LIBINCLUDES := -L$(VCPKGDIR)/openssl_x64-linux/lib -L$(VCPKGDIR)/curl_x64-linux/lib -L/usr/lib/ssl -L/usr/lib
INCLUDES := -I. -I$(INCDIR)
LIBINCLUDES := -L/usr/lib/ssl -L/usr/lib
INC := $(INCLUDES) $(LIBINCLUDES) 
LIBS := -lssl -lcrypto -lcurl

# rules

.PHONY: dump clean distclean

$(TARGET): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INC) -o $(TARGET) $(OBJS) $(LIBS) 

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCS)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $< $(LIBS)

dump:
	@echo "HOME     : $(HOME)"
	@echo "ROOTDIR  : $(ROOTDIR)"
	@echo "OUTDIR   : $(OUTDIR)"
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

distclean: clean
	@rm -vf $(TARGET)

