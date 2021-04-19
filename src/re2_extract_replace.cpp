// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Extract with substitutions
//
// @description
// Like \code{\link{re2_replace}}, except that if the pattern matches,
//   "rewrite" string is returned with substitutions. The
//   non-matching portions of "text" are ignored.
//
// Difference between \code{re2_extract} and \code{\link{re2_replace}}:
// \preformatted{
// > re2_extract("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
// [1] "wunnies!bunny"
// 
// > re2_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
// [1] "wunnies!bunny.pl"
// }
// "\\1" and "\\2" are names of capturing subgroups.
//
// @param text A character string or a character vector where
//   extractions are sought.
// @inheritParams re2_replace
//
// @param \dots The options are (defaults in parentheses):
//
// @return A character string or character vector with extractions.
//   A logical TRUE/FALSE vector may also be returned depending on the
//   options.
//
// @usage re2_extract(text, pattern, rewrite, ...)
//
// @examples
// # Returns extracted string with substitutions
// stopifnot(re2_extract("bunny@wunnies.pl",
//                       "(.*)@([^.]*)",
//                       "\\2!\\1")
//           == "wunnies!bunny")
//
// # Case insensitive
// stopifnot(re2_extract("Bunny@wunnies.pl",
//                       "(b.*)@([^.]*)",
//                       "\\2!\\1",
//                       case_sensitive=FALSE)
//           == "wunnies!Bunny")
// 
// # Max submatch too large (1 match group, 2 submatches needed)
// stopifnot(!re2_extract("foo", "f(o+)", "\\1\\2", logical=TRUE))
// 
// # No match, nothing is extracted
// stopifnot(re2_extract("baz", "bar", "'\\0'") == "")
// 
// # When match fails, logical result is a FALSE
// stopifnot(!re2_extract("baz", "bar", "'\\0'", logical=TRUE))
// 
// [[Rcpp::export]]
SEXP re2_extract_replace(StringVector string, SEXP pattern,
			 std::string& rewrite, bool logical=false) {

  re2::RE2Proxy re2proxy(pattern);
  if ((string.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
      "multiple of pattern vector length" << '\n';
  }

  StringVector outv(string.size());
  LogicalVector lv(string.size());
  for (int i = 0; i < string.size(); i++) {
    int re_idx = i % re2proxy.size();

    if (string(i) == NA_STRING) {
      outv[i] = NA_STRING;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(string(i))); 
    std::string outstr;
    lv[i] = RE2::Extract(strpc, re2proxy[re_idx].get(), rewrite, &outstr);    
    outv[i] = outstr;
  }

  if (logical) return lv;
  return outv;
}
