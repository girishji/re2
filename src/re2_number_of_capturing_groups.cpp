// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Number of capturing subpatterns
//
// Return the number of capturing subpatterns, or -1 if the
// regexp wasn't valid on construction.  The overall match ($0)
// does not count: if the regexp is "(a)(b)", returns 2.
//
// @inheritParams re2_capturing_group_names
//
// @return Number of capturing groups or -1 on error.
//
// @examples
// s <- "(?P<A>expr(?P<B>expr)(?P<C>expr))((expr)(?P<D>expr))"
// stopifnot(re2_number_of_capturing_groups(s) == 6)
// 
// re2p <- re2_re2("directions from (?P<S>.*) to (?P<D>.*)")
// stopifnot(re2_number_of_capturing_groups(re2p) == 2)
//
// [[Rcpp::export(.re2_number_of_capturing_groups)]]
IntegerVector re2_number_of_capturing_groups(SEXP pattern) {
  re2::RE2Proxy container(pattern);
  IntegerVector result(1);
  result[0] = container[0].get().NumberOfCapturingGroups();
  return result;
}
