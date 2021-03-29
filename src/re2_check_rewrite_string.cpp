#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_check_rewrite_string(SEXP pattern, StringVector rewrite,
			      Nullable<List> more_options
			      = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(rewrite.size());
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  StringVector errors(rewrite.size());

  for (int i = 0; i < rewrite.size(); i++) {

    if (rewrite(i) == NA_STRING) {
      errors[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(rewrite(i))); // shallow copy 
    std::string err_str;
    lv[i] = re2proxy.get().CheckRewriteString(strpc, &err_str);
    errors[i] = err_str;
  }

  if (verbose) {
    return List::create(Named("success") = lv,
			Named("error") = errors);
  }
  return lv;
}
