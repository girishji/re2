// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

//' Escape regexp characters
//'
//' Escapes all potentially meaningful regexp characters in
//' regexp string. The returned string, used as a regular expression,
//' will match exactly the original string.  For example,
//' \preformatted{          1.5-2.0?}
//' may become:
//' \preformatted{          1\.5\-2\.0\?}
//'
//' @param unquoted Character string or vector with regexp.
//'
//' @return Character string or vector with escape characters added.
//'   
// [[Rcpp::export]]
CharacterVector re2_quote_meta(CharacterVector unquoted) {
  CharacterVector outv(unquoted.size());
  for (int i=0; i < unquoted.size(); i++) {
    if (unquoted(i) == NA_STRING) {
      outv[i] = NA_STRING;
      continue;
    }
    outv[i] = RE2::QuoteMeta(as<std::string>(unquoted[i]));
  }
  return outv;
}
