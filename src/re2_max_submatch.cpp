// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

// Maximum submatch
//
// Returns the maximum submatch needed for rewrite.
//   For \code{\link{re2_replace}} and \code{\link{re2_extract}} to be
//   successful, the number
//   of matching groups (submatch) has to be at least a many as the
//   maximum group number mentioned in the rewrite string. The latter
//   is returned by this function.
//
// @param rewrite Character string containing rewrite instructions.
// @return A non-negative integer indicating the maximum submatch.
//
// @examples
// stopifnot(re2_max_submatch("foo \\2,\\1") == 2)
// stopifnot(re2_max_submatch("bar \\2: \\5") == 5)
// stopifnot(re2_max_submatch(c("bar \\2: \\5", "\\1 \\9")) == c(5, 9))
//
// [[Rcpp::export(.re2_max_submatch)]]
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

  return List::create(Named("success") = lv, Named("error") = errors);
}

// Range for strings matching a regexp
//
// @description
// Computes range for any strings matching regexp. The min and max can in
// some cases be arbitrarily precise, so the caller gets to specify the
// maximum desired length of string returned.
//
// Assuming \code{re2_possible_match_range} returns successfully, any
// string s that is an anchored match for this regexp satisfies
// \preformatted{  min <= s && s <= max.}
//
// Note that \code{re2_possible_match_range} will only consider the first copy
// of an infinitely repeated element (i.e., any regexp element followed by a '*'
// or
// '+' operator). Regexps with "{N}" constructions are not affected, as those
// do not compile down to infinite repetitions.
//
// @inheritParams re2_replace
// @param maxlen An integer specifying maximum desired length of returned
// string.
// @param \dots If \verb{logical=TRUE} or \verb{l=T} is specified, it
//   returns TRUE if successful and FALSE otherwise.  If
//   \verb{verbose=TRUE} or \verb{v=T} is given, character vector
//   (with two strings) is returned with "min" and "max" strings. In
//   addition, options to \code{\link{re2_re2}} may also applicable.
//
// @return A named character vector with "min" and "max" values.
//   With \verb{verbose} and \verb{logical} options, returns TRUE if
//   successful and FALSE otherwise.
//
// @usage re2_possible_match_range(pattern, maxlen, ...)
//
// @examples
// r <- re2_possible_match_range("abc|def", 10)
// stopifnot(r["min"] == "abc", r["max"] == "def")
//
// r <- re2_possible_match_range("a(b)(c)[d]", 10)
// stopifnot(r["min"] == "abcd", r["max"] == "abcd")
//
// r <- re2_possible_match_range("(abc)+", 10)
// stopifnot(r["min"] == "abc", r["max"] == "abcac")
//
// r <- re2_possible_match_range("(?i)Abcdef", 10)
// stopifnot(r["min"] == "ABCDEF", r["max"] == "abcdef")
//
// [[Rcpp::export(.re2_possible_match_range)]]
SEXP re2_possible_match_range(SEXP pattern, int maxlen, bool logical = false) {

  re2::RE2Proxy re2proxy(pattern);

  StringVector outv(2);
  std::string min, max;
  bool rval = re2proxy[0].get().PossibleMatchRange(&min, &max, maxlen);
  outv[0] = min;
  outv[1] = max;
  outv.attr("names") = StringVector::create("min", "max");

  if (logical) {
    return Rcpp::wrap(rval);
  } else {
    return outv;
  }
}
