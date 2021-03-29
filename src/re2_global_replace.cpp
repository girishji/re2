#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_global_replace(StringVector x, SEXP pattern,
			std::string& rewrite,
			Nullable<List> more_options = R_NilValue) {
  
  re2::RE2Proxy re2proxy(pattern, more_options);
  StringVector sv(x.size());
  IntegerVector cntv(x.size());

  bool count = re2::RE2Proxy::is_count_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
    
  for (int i = 0; i < x.size(); i++) {
    if (x(i) == NA_STRING) {
      sv[i] = NA_STRING;
      cntv[i] = NA_INTEGER;
      continue;
    }

    std::string str = as<std::string>(x(i));
    int cnt = RE2::GlobalReplace(&str, re2proxy.get(), rewrite);
    sv[i] = str;
    if (count || verbose) {
      cntv[i] = cnt;
    }
  }

  if (verbose) {
    return List::create(Named("count") = cntv, Named("result") = sv);
  }
  return count ? cntv : sv;
}
