#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_extract(StringVector x, SEXP pattern,
		 std::string& rewrite,
		 Nullable<List> more_options = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(x.size());

  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  
  StringVector outv(x.size());

  for (int i = 0; i < x.size(); i++) {

    if (x(i) == NA_STRING) {
      outv[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(x(i))); // shallow copy (providing char *)
    std::string outstr;
    bool rval = RE2::Extract(strpc, re2proxy.get(), rewrite, &outstr);
    outv[i] = outstr;
    if (verbose || logical) {
      lv[i] = rval;
    }
  }

  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = outv);
  }
  return logical ? lv : outv;
}
