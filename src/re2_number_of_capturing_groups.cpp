// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2container.h"

using namespace Rcpp;

// [[Rcpp::export]]
IntegerVector re2_number_of_capturing_groups(SEXP pattern) {
  re2::RE2Container container(pattern); // vectorize
  const std::vector<re2::RE2ProxyPtr> &rv = container.get();
  IntegerVector result(rv.size());
  for (std::vector<re2::RE2ProxyPtr>::size_type i = 0;
       i < rv.size(); i++) {
    result[i] = rv[i]->get().NumberOfCapturingGroups();
  }
  return result;
}
