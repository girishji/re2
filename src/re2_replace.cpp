// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

SEXP re2_replace_cpp(StringVector string, SEXP pattern, std::string &rewrite,
                     bool logical);

//' Replace matched pattern in string
//'
//' @description
//' \code{re2_replace} replaces the first match of "pattern" in "string" with
//'   "rewrite" string.
//' \preformatted{
//'   re2_replace("yabba dabba doo", "b+", "d")
//' }
//' will result in "yada dabba doo". \cr
//'
//' \code{re2_replace_all} replaces successive non-overlapping occurrences of
//'   "pattern" in "text" with "rewrite" string.
//' \preformatted{
//'   re2_replace_all("yabba dabba doo", "b+", "d")
//' }
//' will result in "yada dada doo". \cr
//' Replacements are not subject to re-matching.
//' Because \verb{re2_replace_all} only replaces non-overlapping matches,
//'   replacing "ana" within "banana" makes only one replacement, not
//'   two.
//'
//' Vectorized over string and pattern.
//'
//' @inheritParams re2_match
//'
//' @param rewrite Rewrite string. Backslash-escaped
//'   digits (\\1 to \\9) can be used to insert text matching
//'   corresponding parenthesized group from the pattern. \\0
//'   refers to the entire matching text.
//'
//' @return A character vector with replacements.
//' @example inst/examples/replace.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax.
// [[Rcpp::export]]
SEXP re2_replace(StringVector string, SEXP pattern, std::string &rewrite) {
  return re2_replace_cpp(string, pattern, rewrite, false);
}

// [[Rcpp::export(.re2_replace_cpp)]]
SEXP re2_replace_cpp(StringVector string, SEXP pattern, std::string &rewrite,
                     bool logical = false) {

  re2::RE2Proxy re2proxy(pattern);
  CharacterVector cv(string.size());
  LogicalVector lv(string.size());

  if ((string.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
             "multiple of pattern vector length"
          << '\n';
  }
  for (int i = 0; i < string.size(); i++) {
    int re_idx = i % re2proxy.size();

    if (string(i) == NA_STRING) {
      cv[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    std::string str = as<std::string>(string(i));
    bool rval = RE2::Replace(&str, re2proxy[re_idx].get(), rewrite);
    cv[i] = str;
    if (logical) {
      lv[i] = rval;
    }
  }

  return logical ? lv : cv;
}

SEXP re2_replace_all_cpp(StringVector string, SEXP pattern,
                         std::string &rewrite, bool count);

//' @inherit re2_replace
// [[Rcpp::export]]
SEXP re2_replace_all(StringVector string, SEXP pattern, std::string &rewrite) {
  return re2_replace_all_cpp(string, pattern, rewrite, false);
}

// [[Rcpp::export(.re2_replace_all_cpp)]]
SEXP re2_replace_all_cpp(StringVector string, SEXP pattern,
                         std::string &rewrite, bool count = false) {

  re2::RE2Proxy re2proxy(pattern);
  StringVector sv(string.size());
  IntegerVector cntv(string.size());

  if ((string.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
             "multiple of pattern vector length"
          << '\n';
  }
  for (int i = 0; i < string.size(); i++) {
    int re_idx = i % re2proxy.size();

    if (string(i) == NA_STRING) {
      sv[i] = NA_STRING;
      cntv[i] = NA_INTEGER;
      continue;
    }

    std::string str = as<std::string>(string(i));
    int cnt = RE2::GlobalReplace(&str, re2proxy[re_idx].get(), rewrite);
    sv[i] = str;
    if (count) {
      cntv[i] = cnt;
    }
  }

  return count ? cntv : sv;
}
