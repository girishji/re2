#
# Invoke as 'make -f make.local <target>'
#

include ../Makevars

.PHONY: clean
clean:
	cd .. && rm -f $(OFILES) *.o re2.so symbols.rds

.PHONY: buildR
buildR:
	cd ../.. && Rscript -e 'library(Rcpp);compileAttributes()'
	cd ../ && sed -r 's/(.*)more_options = NULL(.*)/\1...\2\n    more_options <- list(...)/g' \
	    ../R/RcppExports.R > ../R/RcppExports.R.tmp
	cd ../ && mv ../R/RcppExports.R.tmp ../R/RcppExports.R
	cd ../../.. && R CMD INSTALL re2

.PHONY: check
check:
	$(MAKE) clean
	cd ../re2google && $(MAKE) clean
	cd ../../.. && R CMD build re2 && R CMD check --as-cran re2_0.1.0.tar.gz
