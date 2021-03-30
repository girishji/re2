// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
XPtr<RE2> re2_re2(std::string& pattern,
		  Nullable<List> more_options = R_NilValue) {

  RE2::Options opt;
  re2::RE2Proxy::modify_options(opt, more_options);

  auto re2ptr = new RE2(pattern, opt);
  if (!(re2ptr->ok())) {
    throw std::invalid_argument(re2ptr->error());
  }
  return XPtr<RE2>(re2ptr);
}
