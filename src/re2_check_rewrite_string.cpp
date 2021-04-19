// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Check suitability of rewrite string
//
// Check if a rewrite string is suitable for use with a regular expression.
//
// Check that the given rewrite string is suitable for use with a
// regular expression. It checks that:
// \itemize{
//   \item The regular expression has enough parenthesized subexpressions
//     to satisfy all of the \\N tokens in rewrite
//   \item The rewrite string doesn't have any syntax errors. E.g.,
//     '\\' followed by anything other than a digit or '\\'.
// }
// A TRUE return value guarantees that \code{\link{re2_replace}} and
//   \code{\link{re2_extract}} won't fail because of a bad rewrite
//   string.
//
// @inheritParams re2_replace
// @param \dots If \verb{verbose=TRUE} or \verb{v=T}, error string is
//   returned (in case of error) along with logical TRUE/FALSE. In addition,
//   options to \code{\link{re2_re2}} are also applicable.
//
// @return TRUE/FALSE or a vector of logical values. With verbose
//   option error strings are also returned in case of error.
//
// @examples
// stopifnot(re2_check_rewrite_string("abc", "foo"));
// stopifnot(!re2_check_rewrite_string("abc", "foo\\"));
// stopifnot(re2_check_rewrite_string("abc", "foo\\0bar"));
// 
// [[Rcpp::export(.re2_check_rewrite_string)]]
SEXP re2_check_rewrite_string(SEXP pattern, StringVector rewrite) {

  re2::RE2Proxy re2proxy(pattern);
  LogicalVector lv(rewrite.size());
  StringVector errors(rewrite.size());

  for (int i = 0; i < rewrite.size(); i++) {

    if (rewrite(i) == NA_STRING) {
      errors[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(rewrite(i))); // shallow copy 
    std::string err_str;
    lv[i] = re2proxy[0].get().CheckRewriteString(strpc, &err_str);
    errors[i] = err_str;
  }

  return List::create(Named("success") = lv,
		      Named("error") = errors);
}
