// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Extract with substitutions
//'
//' @description
//' Like \code{\link{re2_replace}}, except that if the pattern matches,
//'   "rewrite" string is returned with substitutions. The
//'   non-matching portions of "text" are ignored.
//'
//' Difference between \code{re2_extract} and \code{\link{re2_replace}}:
//' \preformatted{
//' > re2_extract("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
//' [1] "wunnies!bunny"
//' 
//' > re2_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
//' [1] "wunnies!bunny.pl"
//' }
//' "\\1" and "\\2" are names of capturing subgroups.
//'
//' @param text A character string or a character vector where
//'   extractions are sought.
//' @inheritParams re2_replace
//'
//' @param \dots The options are (defaults in parentheses):
//'
//' \tabular{lll}{
//'   \verb{logical} \tab (\verb{FALSE}) If TRUE, returns a logical
//'                         result of TRUE iff a match occurred and 
//'                         the extraction happened successfully.
//'                         FALSE otherwise. \cr
//'   \verb{verbose} \tab (\verb{FALSE}) If TRUE, character string
//'                         or vector with extractions is returned
//'                         along with logical result.\cr
//'   \verb{l} \tab (\verb{FALSE}) Same as \verb{logical} above.\cr
//'   \verb{v} \tab (\verb{FALSE}) Same as \verb{verbose} above.\cr
//' }
//' In addition, options under "RE2 Options" section below are also
//'   applicable when \verb{pattern} is given as a character string.
//'    If regexp pattern is precompiled, then options given to
//'   \code{\link{re2_re2}} take precedence.
//'
//' @inheritSection re2_re2 RE2 Options
//'
//' @return A character string or character vector with extractions.
//'   A logical TRUE/FALSE vector may also be returned depending on the
//'   options.
//'
//' @usage re2_extract(text, pattern, rewrite, ...)
//'
//' @examples
//' # Returns extracted string with substitutions
//' stopifnot(re2_extract("bunny@wunnies.pl",
//'                       "(.*)@([^.]*)",
//'                       "\\2!\\1")
//'           == "wunnies!bunny")
//' # Case insensitive
//' stopifnot(re2_extract("Bunny@wunnies.pl",
//'                       "(b.*)@([^.]*)",
//'                       "\\2!\\1",
//'                       case_sensitive=FALSE)
//'           == "wunnies!Bunny")
//' # Max submatch too large (1 match group, 2 submatches needed)
//' stopifnot(!re2_extract("foo", "f(o+)", "\\1\\2", logical=TRUE))
//' # No match, nothing is extracted
//' stopifnot(re2_extract("baz", "bar", "'\\0'") == "")
//' # When match fails, logical result is a FALSE
//' stopifnot(!re2_extract("baz", "bar", "'\\0'", logical=TRUE))
//' # A vector parameter
//' stopifnot(re2_extract(c("Bunny@wunnies.pl", "cargo@cult.org"),
//'                       "(.*)@([^.]*)", "\\2!\\1")
//'           == c("wunnies!Bunny", "cult!cargo"))
//'
//' @inheritSection re2_re2 Regexp Syntax
//'
//' @seealso \code{\link{re2_re2}}, \link{re2_regexp},
//'   \code{\link{re2_replace}},
//'   \code{\link{re2_match}}, \code{\link{re2_global_replace}}.
// [[Rcpp::export]]
SEXP re2_extract(StringVector text, SEXP pattern,
		 std::string& rewrite,
		 Nullable<List> more_options = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(text.size());

  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  
  StringVector outv(text.size());

  for (int i = 0; i < text.size(); i++) {

    if (text(i) == NA_STRING) {
      outv[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(text(i))); // shallow copy (providing char *)
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
