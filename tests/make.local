# XXX: Copyright

include GNUmakefile.RInside
CXXFLAGS += -O0 -g
CPPFLAGS += -DRE2_R_TEST

# Create symlink farm for building. VPATH is not an option since
#   we want to retain the option to build and test re2 distro without
#   Rcpp and RInside. Using GNU vpath directive to only locate
#   source files is an option but it requires modifying the Makefile
#   that came with re2 distro. Could also use lndir here.
re2dir = ../src/re2google
re2files = $(patsubst %,$(re2dir)/%,Makefile libre2.symbols libre2.symbols.darwin runtests)
.PHONY: symlinks
symlinks: re2google
re2google:
	@mkdir -p $@
	@find ../src/re2google/re2 -type d | sed 's/..\/src\///' | xargs -I {} mkdir -p {}
	@find ../src/re2google/util -type d | sed 's/..\/src\///' | xargs -I {} mkdir -p {}
	@find ../src/re2google/re2 ../src/re2google/util -type f | while read x; do \
			dstf=$$(echo $$x | sed 's/..\/src\///'); \
			srcf=$$(echo $$(pwd)/$$x | sed 's/tests\/..\///'); \
			ln -sf $$srcf $$dstf; \
		done;
	@rm -f re2google/re2/re2.cc
	@ln -sf $$(pwd)/re2_R.cc re2google/re2/re2.cc
	@rm -f re2google/re2/re2.h
	@ln -sf $$(pwd)/re2_R.h re2google/re2/re2.h
	@for f in $(re2files); do \
		ln -sf $$(pwd)/$$f re2google/; \
	done

.PHONY: cleanall
cleanall:
	rm -rf re2google

# Only run tests for static and debug dirs, not for shared object (so) dir,
#   since the tests are simply repeated.
.PHONY: test
test: re2google
	@$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' \
		LDFLAGS='$(LDLIBS)' -C re2google debug-test static-test

.PHONY: bigtest
bigtest: re2google
	@$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' \
		LDFLAGS='$(LDLIBS)' -C re2google static-bigtest

# Defer targets to the makefile from re2 distro
# https://www.gnu.org/software/make/manual/html_node/Overriding-Makefiles.html
declared = symlinks cleanall test bigtest
%: force
ifeq (,$(filter $(MAKECMDGOALS),$(declared)))
ifeq ($(MAKECMDGOALS),)
	@$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDLIBS)' -C re2google
else
	@if echo $(MAKECMDGOALS) | grep $@ > /dev/null 2>&1; then \
		$(MAKE) CPPFLAGS='$(CPPFLAGS)' CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDLIBS)' -C re2google $@; \
	fi
endif
endif

force: re2google;

