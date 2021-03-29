// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_replace(StringVector x, SEXP pattern,
		 std::string& rewrite,
		 Nullable<List> more_options = R_NilValue) {
  
  re2::RE2Proxy re2proxy(pattern, more_options);
  CharacterVector cv(x.size());
  LogicalVector lv(x.size());
  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);

  for (int i = 0; i < x.size(); i++) {
    if (x(i) == NA_STRING) {
      cv[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    std::string str = as<std::string>(x(i));
    bool rval = RE2::Replace(&str, re2proxy.get(), rewrite);
    cv[i] = str;
    if (verbose || logical) {
      lv[i] = rval;
    }
  }

  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = cv);
  }
  return logical ? lv : cv;
}
