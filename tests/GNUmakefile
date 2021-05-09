##
## A simple makefile for unit testing
##

include GNUmakefile.RInside
CXXFLAGS += -O0 -g
CPPFLAGS += -DRE2_R_TEST

# Create symlink farm for building. VPATH is not an option since
#   we want to retain the option to build and test re2 distro without
#   Rcpp and RInside. Using GNU vpath directive to only locate
#   source files is an option but it requires modifying the Makefile
#   that came with re2 distro. Could also use lndir here.
re2dir = ../src/re2google
re2testdir = re2google_test
re2files = $(patsubst %,$(re2dir)/%,GNUmakefile libre2.symbols libre2.symbols.darwin runtests)

.PHONY: symlinks
symlinks: $(re2testdir)
$(re2testdir):
	@mkdir -p $@
	@find $(re2dir)/re2 -type d | sed 's/..\/src\/re2google\///' | xargs -I {} mkdir -p $@/{}
	@find $(re2dir)/util -type d | sed 's/..\/src\/re2google\///' | xargs -I {} mkdir -p $@/{}
	@find $(re2dir)/re2 $(re2dir)/util -type f | while read x; do \
			dstf=$$(echo $$x | sed 's/..\/src\/re2google\///'); \
			srcf=$$(echo $$(pwd)/$$x | sed 's/tests\/..\///'); \
			ln -sf $$srcf $@/$$dstf; \
		done;
	@rm -f $@/re2/re2.cc
	@ln -sf $$(pwd)/re2_R.cc $@/re2/re2.cc
	@rm -f $@/re2/re2.h
	@ln -sf $$(pwd)/re2_R.h $@/re2/re2.h
	@for f in $(re2files); do \
		ln -sf $$(pwd)/$$f $@/; \
	done

.PHONY: cleanall
cleanall:
	rm -rf $(re2testdir)

# Only run tests for static and debug dirs, not for shared object (so) dir,
#   since the tests are simply repeated.
.PHONY: test
test: $(re2testdir)
	@$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' \
		LDFLAGS='$(LDLIBS)' -C $(re2testdir) debug-test static-test

.PHONY: bigtest
bigtest: $(re2testdir)
	@$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' \
		LDFLAGS='$(LDLIBS)' -C $(re2testdir) static-bigtest

# Defer targets to the makefile from re2 distro
# https://www.gnu.org/software/make/manual/html_node/Overriding-Makefiles.html
declared = symlinks cleanall test bigtest
%: force
ifeq (,$(filter $(MAKECMDGOALS),$(declared)))
ifeq ($(MAKECMDGOALS),)
	@$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDLIBS)' -C $(re2testdir)
else
	@if echo $(MAKECMDGOALS) | grep $@ > /dev/null 2>&1; then \
		$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDLIBS)' -C $(re2testdir) $@; \
	fi
endif
endif

force: $(re2testdir);

