// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

SEXP re2_extract_replace_cpp(StringVector string, SEXP pattern,
                             std::string &rewrite, bool logical);

//' Extract with substitutions
//'
//' @description
//' Like \code{\link{re2_replace}}, except that if the pattern matches,
//'   "rewrite" string is returned with substitutions. The
//'   non-matching portions of "text" are ignored.
//'
//' Difference between \code{re2_extract_replace} and \code{\link{re2_replace}}:
//' \preformatted{
//' > re2_extract_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
//' [1] "wunnies!bunny"
//'
//' > re2_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
//' [1] "wunnies!bunny.pl"
//' }
//' "\\1" and "\\2" are names of capturing subgroups.
//'
//' Vectorized over string and pattern.
//'
//' @inheritParams re2_replace
//'
//' @return A character vector with extractions.
//'
//' @example inst/examples/extract_replace.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for regular expression syntax. See
//'   \code{\link{re2_replace}} and \code{\link{re2_replace_all}} to replace
//'   pattern in place.
// [[Rcpp::export]]
SEXP re2_extract_replace(StringVector string, SEXP pattern,
                         std::string &rewrite) {
  return re2_extract_replace_cpp(string, pattern, rewrite, false);
}

// [[Rcpp::export(.re2_extract_replace_cpp)]]
SEXP re2_extract_replace_cpp(StringVector string, SEXP pattern,
                             std::string &rewrite, bool logical = false) {
  re2::RE2Proxy re2proxy(pattern);
  if ((string.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
             "multiple of pattern vector length"
          << '\n';
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

  if (logical)
    return lv;
  return outv;
}
