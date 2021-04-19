// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Find the presence of a pattern in string(s)
//'
//' @description
//' Equivalent to grepl(pattern, x). Vectorized over
//'   string. For the equivalent of
//'   grep(pattern, x) use which(re2_detect(x, pattern)).
//'
//' @inheritParams re2_match_cpp
//'
//' @return A logical vector. TRUE if match is found, FALSE if not.
//'
//' @usage
//'   re2_detect(string, pattern)
//'   re2_detect(string, re2_regexp(pattern, ...)))
//'
//' @examples
//' 
//' ## Character vector input
//' s <- c("barbazbla", "foobar", "this is a test")
//' pat <- "(foo)|(bar)baz"
//' re2_detect(s, pat)
//' 
//' ## Use precompiled regexp
//' re <- re2_regexp("(foo)|(bAR)baz", case_sensitive=FALSE)
//' re2_detect(s, re)
//' stopifnot(re2_detect(s, re) == c(TRUE, TRUE, FALSE))
//'      
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax, and 
//'   \code{\link{re2_match_cpp}} which this function wraps.
//'
// [[Rcpp::export]]
LogicalVector re2_detect(StringVector string, SEXP pattern) {
  re2::RE2Proxy re2proxy(pattern);
  StringVector& vstring = string;
  LogicalVector result(vstring.size());

  if ((vstring.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
      "multiple of pattern vector length" << '\n';
  }
    
  for (int i = 0; i < vstring.size(); i++) {
    if (vstring(i) == NA_STRING) {
      result(i) = NA_LOGICAL;
      continue;
    }
    int re_idx = i % re2proxy.size();
    re2::StringPiece text(R_CHAR(vstring(i)));

    if (re2proxy[re_idx].get().Match(text, 0, text.size(),
				     RE2::UNANCHORED,
				     nullptr, 0)) {
      result(i) = true;
    } else {
      result(i) = false;
    }
  }
  return result;
}

// [[Rcpp::export]]
IntegerVector re2_which(StringVector string, SEXP pattern) {
  LogicalVector vec = re2_detect(string, pattern);
  std::vector<int> res;
  for (int i = 0; i < vec.size(); i++) {
    if (vec(i)) {
      res.push_back(i + 1);
    }
  }
  return wrap(res);  
}

// [[Rcpp::export]]
StringVector re2_subset(StringVector string, SEXP pattern) {
  LogicalVector vec = re2_detect(string, pattern);
  std::vector<std::string> res;
  for (int i = 0; i < vec.size(); i++) {
    if (vec(i)) {
      res.push_back(as<std::string>(string(i)));
    }
  }
  return wrap(res);      
}
