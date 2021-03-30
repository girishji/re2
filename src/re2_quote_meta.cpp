// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

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
