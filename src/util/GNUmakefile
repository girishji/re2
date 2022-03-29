#
# Invoke as 'make -f make.local <target>'
#

include ../Makevars


# Install from tarball. When tarball is created by 'build'
#    command, it ignores files from .Rbuildignore.
#    "R CMD INSTALL re2" does not consider .Rbuildignore
#    file. Using tarball, it takes longer
#    to build (no incremental build) but it is closer
#    to user installation. It will catch if some file
#    is accidentally ignored by .Rbuildignore

tarball=re2_0.1.2.tar.gz
namespacefile=../../NAMESPACE

all: test

.PHONY: restore
restore:
	if grep "exportPattern(\"^(" $(namespacefile) >/dev/null; then \
		grep -v exportPattern $(namespacefile) > /tmp/.namespace; \
		echo "exportPattern(\"^[[:alpha:]]+\")" >> /tmp/.namespace; \
		mv /tmp/.namespace $(namespacefile); \
	fi

.PHONY: clean
clean:
	cd .. && rm -f $(OFILES) *.o re2.so symbols.rds
	cd ../../.. && rm -f $(tarball)
	cd ../.. && rm -f vignettes/*.html

.PHONY: cleanDoc
cleanDoc:
	rm -f ../../man/*.Rd

.PHONY: fix_R_exports
fix_R_exports:
	sed -r -e 's/(.*)more_options = NULL(.*)/\1...\2\n    more_options <- list(...)/g' \
		../../R/RcppExports.R > /tmp/RcppExports.R.tmp
	if ! cmp  ../../R/RcppExports.R  /tmp/RcppExports.R.tmp > /dev/null; then \
		mv /tmp/RcppExports.R.tmp ../../R/RcppExports.R ; \
	fi

.PHONY: buildR
buildR: 
	$(MAKE) restore
	$(MAKE) clean cleanDoc
	cd ../re2google && $(MAKE) clean
	cd ../.. && Rscript --vanilla -e 'library(Rcpp);compileAttributes()'
	cd ../.. && Rscript --vanilla -e 'library(roxygen2);roxygen2::roxygenize(roclets="rd")'
	$(MAKE) fix_R_exports
	cd ../../tools/ && python3 convert.py > ../man/re2_syntax.Rd
	$(MAKE) clean # clean up roxygen created binaries
	cd ../re2google && $(MAKE) clean
	cd ../../tests && $(MAKE) cleanall
	cd ../../.. && R CMD build re2

README.md: README.Rmd
	cd ../.. && Rscript -e 'library(rmarkdown); rmarkdown::render("README.Rmd", NULL)'

.PHONY: installR
installR: 
	$(MAKE) buildR
	cd ../../.. && R CMD INSTALL $(tarball)

.PHONY: prepare
prepare:
	cd ../re2google && $(MAKE) clean
	cd ../.. && Rscript --vanilla -e 'library(Rcpp);compileAttributes()'
	$(MAKE) fix_R_exports

.PHONY: quick
quick: 
	$(MAKE) restore
	$(MAKE) clean cleanDoc
	$(MAKE) prepare
	cd ../../tests && $(MAKE) cleanall # .Rbuildignore does not always work
	cd ../../.. && R CMD build --no-build-vignettes --no-manual re2
	cd ../../.. && R CMD INSTALL $(tarball)

.PHONY: test
test:
	grep -v exportPattern $(namespacefile) > /tmp/.namespace
	echo 'exportPattern("^(\\\\.)*[[:alpha:]]+")' >> /tmp/.namespace
	mv /tmp/.namespace $(namespacefile)
	$(MAKE) prepare
	cd ../../.. && R CMD INSTALL re2

.PHONY: check
check: 
	$(MAKE) buildR
	cd ../../.. && R CMD check --as-cran $(tarball)

.PHONY: rox
rox: 
	$(MAKE) restore
	cd ../.. && Rscript --vanilla -e 'library(Rcpp);compileAttributes()'
	cd ../.. && Rscript --vanilla -e 'library(roxygen2);roxygen2::roxygenize(roclets="rd")'
	$(MAKE) fix_R_exports
	cd ../../tools/ && python3 convert.py > ../man/re2_syntax.Rd
	cd ../../.. && R CMD INSTALL re2

.PHONY: examples
examples: 
	$(MAKE) restore
	$(MAKE) rox
	cd ../.. && Rscript --vanilla -e 'library(devtools);run_examples()'

# R CMD check passes even when Rout files have differences. Not using this.
.PHONY: Rout.save
Rout.save:
	if [[ -f ../../../re2.Rcheck/re2-Ex.Rout ]]; then \
	    cp ../../../re2.Rcheck/re2-Ex.Rout ../../tests/Examples/re2-Ex.Rout.save; \
	fi

