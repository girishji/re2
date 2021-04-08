// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Maximum submatch
//'
//' Returns the maximum submatch needed for rewrite.
//'   For \code{\link{re2_replace}} and \code{\link{re2_extract}} to be
//'   successful, the number
//'   of matching groups (submatch) has to be at least a many as the
//'   maximum group number mentioned in the rewrite string. The latter
//'   is returned by this function.
//'   
//' @param rewrite Character string containing rewrite instructions.
//' @return A non-negative integer indicating the maximum submatch.
//'
//' @examples
//' stopifnot(re2_max_submatch("foo \\2,\\1") == 2)
//' stopifnot(re2_max_submatch("bar \\2: \\5") == 5)
//' stopifnot(re2_max_submatch(c("bar \\2: \\5", "\\1 \\9")) == c(5, 9))
//'
//' @seealso \code{\link{re2_number_of_capturing_groups}}, \code{\link{re2_replace}},
//'   \code{\link{re2_global_replace}}, \code{\link{re2_extract}}.
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
