#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_max_submatch(StringVector rewrite) {

  IntegerVector ms(rewrite.size());

  for (int i = 0; i < rewrite.size(); i++) {
    if (rewrite(i) == NA_STRING) {
      ms[i] = NA_INTEGER;
      continue;
    }
    re2::StringPiece strpc(R_CHAR(rewrite(i))); // shallow copy 
    ms[i] = RE2::MaxSubmatch(strpc);
  }
  return ms;
}
