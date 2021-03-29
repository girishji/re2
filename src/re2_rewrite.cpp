#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_rewrite(SEXP pattern,
		 StringVector rewrite,
		 StringVector vec,
		 int veclen,
		 Nullable<List> more_options = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(rewrite.size());
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  StringVector res(rewrite.size());

  re2::StringPiece vec_sp[vec.size()];
  for (int i = 0; i < vec.size(); i++) {
    vec_sp[i] = re2::StringPiece(R_CHAR(vec(i)));
  }
  
  for (int i = 0; i < rewrite.size(); i++) {

    if (rewrite(i) == NA_STRING) {
      res[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(rewrite(i)));
    std::string out;
    lv[i] = re2proxy.get().Rewrite(&out, strpc, vec_sp, veclen);
    res[i] = out;
  }

  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = res);
  }
  return logical ? lv : res;
}
