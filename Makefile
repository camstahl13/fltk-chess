# Template Makefile for C++ projects
# Used ideas (and some code) from these resources:
# http://nuclear.mutantstargoat.com/articles/make/
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#combine

# Define the desired name of your executable.
PROG = prog
# Define valid source file extensions.
SRC_EXT = cpp cxx cc

# Don't pollute source directory with object files.
OBJDIR = obj
# Note: Change DEPDIR to something without leading dot if we don't want it
# hidden by default.
DEPDIR = .d
# Get list of source files in current directory, and use it to generate
# corresponding lists of object files.
SRC = $(wildcard $(addprefix *.,$(SRC_EXT)))
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(basename $(SRC))))

# The following preprocessor, compiler and linker flags, may be overridden by
# customization makefiles.
CPPFLAGS =
CXXFLAGS =
LDFLAGS =

# Pull in any makefile customizations in current directory.
# Intent: Allow a project to override any of the variables set above in one or
# more project (and perhaps module-) specific files.
# Note: You can disable a customization file by hiding: e.g.,
# mv foo_custom.mk foo_custom.mk.hide
-include *_custom.mk custom.mk

# DEFAULT GOAL
$(PROG) : $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Dependency generation
# Make won't create the build artifact directories for us.
$(shell mkdir -p $(DEPDIR) $(OBJDIR) >/dev/null)
# Note: -MP adds an extra target (see Make "force targets") for each dependency
# other than the primary source file, the purpose of which is to prevent error
# when a header listed in the .d has been removed from the project.
# Note: The 2 step creation of the .d is part of a "make before break" strategy
# that ensures a .d isn't left in a corrupted state if compilation fails.
# Note: The `touch' of the .o is required because apparently some compilers
# produce an object file with an older timestamp than the generated .d.
DEPFLAGS = -MMD -MP -MF $(DEPDIR)/$*.Td
DEPGEN = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

# Disable several builtin implicit rules to prevent their taking precedence
# over the implicit rules we're about to define.
# Explanation: When the .d doesn't exist, the builtin implicit will take
# precedence, since the .d is made with an implicit rule, thereby creating a
# "chain" of implicit rules, causing Make to prefer the builtin implicit, which
# doesn't require any files that don't already exist.
%.o : %.cpp
%.o : %.cxx

# Define implicit rules for each C++ source file extension we support.
# Note: Use a template consisting of eval within a foreach to scale to many C++
# source extensions. The output of the template should be rules that look like
# this...
#$(OBJDIR)/%.o : %.cpp $(DEPDIR)/%.d
#	$(CXX) $(CPPFLAGS) $(DEPFLAGS) $(CXXFLAGS) -c -o $@ $<
#	$(DEPGEN)

define compile_tmpl =
$$(OBJDIR)/%.o : %.$(1) $$(DEPDIR)/%.d
	$$(CXX) $$(CPPFLAGS) $$(DEPFLAGS) $$(CXXFLAGS) -c -o $$@ $$<
	$$(DEPGEN)
endef

# Generate one rule for each supported C++ source extension.
$(foreach ext,$(SRC_EXT),$(eval $(call compile_tmpl,$(ext))))


# Prevent missing .d from causing error.
# Explanation: The implicit rules we define for .o files depend on .d files. If
# the .d file doesn't exist, Make will attempt to remake the .d before remaking
# the .o. If Make finds no rule for making the .d, it will generate an error.
# But since the .d is built as a side effect of compilation, there's
# really nothing for a .d rule to do, so we define a do-nothing rule that will
# trick Make into thinking the .d has been brought up to date, though of course
# it won't be up to date till the subsequent compilation has completed.
$(DEPDIR)/%.d : ;

# Mark the .d files "precious".
# Explanation: Although the rules to make the .o files actually generate the .d
# files, from Make's perspective, the .d files are made by the do-nothing rule
# above, and are therefore considered intermediate files, since they're
# generated in a chain of implicit rules. Thus, if they're not marked
# "precious", they'll be deleted by Make, which would render their creation
# pointless.
.PRECIOUS: $(DEPDIR)/%.d

# Include any *existing* dependency fragments.
# Explanation: Some dependency strategies depend on the include of a .d to
# force its initial creation, but the current approach makes the .d a
# dependency of the .o, which means the .o gets remade whenever the .d doesn't
# exist, and remaking the .o generates the .d as a side effect.
-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC))))

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(PROG)

.PHONY: cleandep
cleandep:
	rm -f $(DEPDIR)/*.d

.PHONY: realclean
realclean: clean cleandep
