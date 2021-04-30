// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

//' Find the presence of a pattern in string(s)
//'
//' @description
//' Equivalent to grepl(pattern, x). Vectorized over
//'   string and pattern. For the equivalent of
//'   grep(pattern, x) see \code{\link{re2_which}}.
//'
//' @inheritParams re2_match
//'
//' @return A logical vector. TRUE if match is found, FALSE if not.
//'
//' @example inst/examples/detect.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax, and
//'   \code{\link{re2_match}} to extract matched groups.
//'
// [[Rcpp::export]]
LogicalVector re2_detect(StringVector string, SEXP pattern) {
  re2::RE2Proxy re2proxy(pattern);
  StringVector &vstring = string;
  LogicalVector result(vstring.size());

  if ((vstring.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
             "multiple of pattern vector length"
          << '\n';
  }

  for (int i = 0; i < vstring.size(); i++) {
    if (vstring(i) == NA_STRING) {
      result(i) = NA_LOGICAL;
      continue;
    }
    int re_idx = i % re2proxy.size();
    re2::StringPiece text(R_CHAR(vstring(i)));

    if (re2proxy[re_idx].get().Match(text, 0, text.size(), RE2::UNANCHORED,
                                     nullptr, 0)) {
      result(i) = true;
    } else {
      result(i) = false;
    }
  }
  return result;
}

//' Select strings that match, or find their positions
//'
//' @description
//' \code{re2_subset} returns strings that match a pattern.
//' \code{re2_which} is equivalent to grep(pattern, x). It returns
//'   position of string that match a pattern. Vectorized over
//'   string and pattern. For the equivalent of
//'   grepl(pattern, x) see \code{\link{re2_detect}}.
//'
//' @inheritParams re2_match
//'
//' @return \code{re2_subset} returns a character vector, and \code{re2_which}
//'   returns an integer vector.
//'
//' @example inst/examples/which.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax, and
//'   \code{\link{re2_detect}} to find presence of a pattern (grep).
//'
// [[Rcpp::export]]
IntegerVector re2_which(StringVector string, SEXP pattern) {
  LogicalVector vec = re2_detect(string, pattern);
  std::vector<int> res;
  for (int i = 0; i < vec.size(); i++) {
    if (vec(i) && vec(i) != NA_LOGICAL) {
      res.push_back(i + 1);
    }
  }
  return wrap(res);
}

//' @inherit re2_which
// [[Rcpp::export]]
StringVector re2_subset(StringVector string, SEXP pattern) {
  LogicalVector vec = re2_detect(string, pattern);
  std::vector<std::string> res;
  for (int i = 0; i < vec.size(); i++) {
    if (vec(i) && vec(i) != NA_LOGICAL) {
      res.push_back(as<std::string>(string(i)));
    }
  }
  return wrap(res);
}
