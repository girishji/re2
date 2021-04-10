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

tarball=re2_0.1.0.tar.gz
namespacefile=../../NAMESPACE

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

.PHONY: buildR
buildR: restore
	$(MAKE) clean cleanDoc
	cd ../re2google && $(MAKE) clean
	cd ../.. && Rscript -e 'library(Rcpp);compileAttributes()'
	cd ../.. && Rscript -e 'library(roxygen2);roxygen2::roxygenize(roclets="rd")'
	cd ../ && sed -r 's/(.*)more_options = NULL(.*)/\1...\2\n    more_options <- list(...)/g' \
	    ../R/RcppExports.R > ../R/RcppExports.R.tmp
	cd ../ && mv ../R/RcppExports.R.tmp ../R/RcppExports.R
	python3 convert.py > ../../man/re2_syntax.Rd
	$(MAKE) clean # clean up roxygen created binaries
	cd ../../tests && $(MAKE) cleanall # .Rbuildignore is useless
	cd ../../.. && R CMD build re2

.PHONY: installR
installR: restore
	$(MAKE) buildR
	cd ../../.. && R CMD INSTALL $(tarball)

.PHONY: quick _quick
quick: restore _quick
_quick:
	$(MAKE) clean cleanDoc
	cd ../re2google && $(MAKE) clean
	cd ../.. && Rscript -e 'library(Rcpp);compileAttributes()'
	cd ../ && sed -r 's/(.*)more_options = NULL(.*)/\1...\2\n    more_options <- list(...)/g' \
	    ../R/RcppExports.R > ../R/RcppExports.R.tmp
	cd ../ && mv ../R/RcppExports.R.tmp ../R/RcppExports.R
	cd ../../tests && $(MAKE) cleanall # .Rbuildignore is useless
	cd ../../.. && R CMD build --no-build-vignettes --no-manual re2
	cd ../../.. && R CMD INSTALL $(tarball)

.PHONY: test
test:
	grep -v exportPattern $(namespacefile) > /tmp/.namespace
	echo 'exportPattern("^(\\\\.)*[[:alpha:]]+")' >> /tmp/.namespace
	mv /tmp/.namespace $(namespacefile)
	$(MAKE) _quick

.PHONY: check
check: restore
	$(MAKE) buildR
	cd ../../.. && R CMD check --as-cran $(tarball)

.PHONY: rox
rox: restore
	cd ../.. && Rscript -e 'library(Rcpp);compileAttributes()'
	cd ../.. && Rscript -e 'library(roxygen2);roxygen2::roxygenize(roclets="rd")'
	cd ../ && sed -r 's/(.*)more_options = NULL(.*)/\1...\2\n    more_options <- list(...)/g' \
	    ../R/RcppExports.R > ../R/RcppExports.R.tmp
	cd ../ && mv ../R/RcppExports.R.tmp ../R/RcppExports.R
	python3 convert.py > ../../man/re2_syntax.Rd
	cd ../../.. && R CMD INSTALL re2

.PHONY: examples
examples: restore
	$(MAKE) rox
	cd ../.. && Rscript -e 'library(devtools);run_examples()'
