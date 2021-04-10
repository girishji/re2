// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

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
// Note that \code{re2_possible_match_range} will only consider the first copy of an
// infinitely repeated element (i.e., any regexp element followed by a '*' or
// '+' operator). Regexps with "{N}" constructions are not affected, as those
// do not compile down to infinite repetitions.
//
// @inheritParams re2_replace
// @param maxlen An integer specifying maximum desired length of returned string.
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
// @seealso \code{\link{re2_re2}}, \code{\link{re2_replace}},
//   \code{\link{re2_match}}, \code{\link{re2_global_replace}},
//   \code{\link{re2_extract}}.
// [[Rcpp::export(.re2_possible_match_range)]]
SEXP re2_possible_match_range(SEXP pattern,
			      int maxlen,
			      Nullable<List> more_options
			      = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);

  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  
  StringVector outv(2);
  std::string min, max;
  bool rval = re2proxy.get().PossibleMatchRange(&min, &max, maxlen);
  outv[0] = min;
  outv[1] = max;
  outv.attr("names") = StringVector::create("min", "max");
    
  if (verbose) {
    return List::create(Named("success") = rval,
			Named("result") = outv);
  }
  if (logical) {
    return Rcpp::wrap(rval);
  } else {
    return outv;
  }
}
