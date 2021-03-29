// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_possible_match_range(SEXP pattern,
			      int maxlen,
			      Nullable<List> more_options
			      = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);

  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  
  StringVector outv(2);
  std::string min, max;
  bool rval = re2proxy.get().PossibleMatchRange(&min, &max, maxlen);
  outv[0] = min;
  outv[1] = max;
  outv.attr("names") = StringVector::create("min", "max");
    
  if (verbose) {
    return List::create(Named("success") = rval,
			Named("result") = outv);
  }
  if (logical) {
    return Rcpp::wrap(rval);
  } else {
    return outv;
  }
}
