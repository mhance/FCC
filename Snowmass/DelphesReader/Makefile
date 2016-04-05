MAKEFLAGS = --no-print-directory -r -s

include $(shell root-config --prefix)/etc/Makefile.arch

LIBRARY = DelphesReader
OBJDIR  = obj
SRCDIR  = src
INCDIR  = include
DEPDIR  = dep
BINDIR = bin

BASEDIR=$(shell pwd)/..

DELPHES_DIR=$(BASEDIR)/externals/Delphes
DELPHESEXTERNAL_DIR=$(DELPHES_DIR)/external
EXROOT_DIR=$(DELPHESEXTERNAL_DIR)/ExRootAnalysis

VPATH    += $(OBJDIR) $(SRCDIR)
INCLUDES += -I$(DELPHES_DIR) -I$(DELPHES_DIR)/classes -I$(DELPHESEXTERNAL_DIR) -I$(EXROOT_DIR) -I./ -I./include/
CXXFLAGS += -Wall -Wno-overloaded-virtual -Wno-unused $(USERCXXFLAGS)

SHLIBFILE = lib/lib$(LIBRARY).$(DllSuf)

LDFLAGS += $(USERLDFLAGS) $(ROOTLIBS)  -lTreePlayer -L$(DELPHES_DIR) -lDelphes -Wl,-rpath $(DELPHES_DIR) -Wl,-rpath ./lib

default: all
all: core # scripts
core: shlib DRMain
scripts: runCEws train_MVA classify choose_operating_point

HLIST   = $(wildcard $(INCDIR)/*.h)
LIBCPPLIST = $(wildcard $(SRCDIR)/*.$(SrcSuf))
RUNCPPLIST = DRMain.$(SrcSuf)
CPPLIST = $(RUNCPPLIST) $(LIBCPPLIST)

# List of all object files to build
LIBOLIST = $(patsubst %.$(SrcSuf),%.o,$(notdir $(LIBCPPLIST)))
RUNOLIST = $(patsubst %.$(SrcSuf),%.o,$(notdir $(RUNCPPLIST)))


# Implicit rule to compile all sources
%.o : %.$(SrcSuf)
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJDIR)/$(notdir $@) $(INCLUDES)

$(OBJDIR)/%.o : %.$(SrcSuf)
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJDIR)/$(notdir $@) $(INCLUDES)


DRMain: $(OBJDIR)/DRMain.$(ObjSuf)
	@echo "Compiling $^"
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(BINDIR)/$@ $^ -L./lib/ -l$(LIBRARY) 


runCEws: scripts/runCEws.C
	@echo "Compiling $^"
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -I scripts $(ROOTLIBS) -o $(BINDIR)/$@ $^ -lRooStats -lHistFactory

train_MVA: scripts/train_MVA.C
	@echo "Compiling $^"
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(ROOTLIBS) -o $(BINDIR)/$@ $^ -lTMVA

classify: scripts/classify.C
	@echo "Compiling $^"
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(ROOTLIBS) -o $(BINDIR)/$@ $^ -lTMVA

choose_operating_point: scripts/choose_operating_point.C
	@echo "Compiling $^"
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(ROOTLIBS) -o $(BINDIR)/$@ $^ -lTMVA -lRooStats -D__ROOFIT_NOBANNER


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(foreach var,$(notdir $(CPPLIST:.$(SrcSuf)=.d)),$(DEPDIR)/$(var))
endif
endif

$(DEPDIR)/%.d: %.$(SrcSuf)
	@mkdir -p $(DEPDIR)
	if test -f $< ; then \
		echo "Making $(@F)"; \
		$(SHELL) -ec '$(CPP) -MM $(CXXFLAGS) $(INCLUDES) $< | sed '\''/Cstd\/rw/d'\'' > $@'; \
	fi

# Rule to combine objects into a unix shared library
$(SHLIBFILE): $(LIBOLIST)
	@echo "Making shared library: $(SHLIBFILE)"
	@mkdir -p lib
	@rm -f $(SHLIBFILE)
ifneq (,$(findstring macosx,$(ARCH)))
	@$(LD) $(LDFLAGS) $(USERLDFLAGS) -dynamiclib -single_module -undefined \
		dynamic_lookup $(addprefix $(OBJDIR)/,$(LIBOLIST)) \
		-o $(SHLIBFILE)
else
	@$(LD) $(LDFLAGS) $(USERLDFLAGS) $(SOFLAGS) $(addprefix $(OBJDIR)/,$(LIBOLIST)) \
		-o $(SHLIBFILE)
endif


# Useful build targets
shlib: $(SHLIBFILE)
lib: shlib

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(SHLIBFILE)
	@rm -f $(SFRAME_LIB_PATH)/lib$(LIBRARY).so

distclean:
	@rm -rf $(OBJDIR)
	@rm -rf $(DEPDIR)
	@rm -f $(SHLIBFILE)
	@rm -f $(SFRAME_LIB_PATH)/lib$(LIBRARY).so
	@rm -f $(SFRAME_LIB_PATH)/$(LIBRARY).par

.PHONY : shlib default clean

