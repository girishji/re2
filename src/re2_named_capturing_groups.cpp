// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Group names to capturing indices
//
// Return a map from names to capturing indices.
// The map records the index of the leftmost group
// with the given name.
//
// @inheritParams re2_capturing_group_names
//
// @return A named character vector reflecting the mapping from
//   group names to capturing indices. Or, a list of mappings
//   if parameter is a vector.
//
// @examples
// res <- re2_named_capturing_groups("(hello world)")
// stopifnot(is.na(res))
// 
// res <- re2_named_capturing_groups("directions from (?P<S>.*) to (?P<D>.*)")
// stopifnot(res["S"] == 1)
// stopifnot(res["D"] == 2)
// 
// s <- "(?P<A>expr(?P<B>expr)(?P<C>expr))((expr)(?P<D>expr))"
// res <- re2_named_capturing_groups(s)
// stopifnot(res["A"] == 1)
// stopifnot(res["B"] == 2)
// stopifnot(res["C"] == 3)
// stopifnot(res["D"] == 6)
//
// [[Rcpp::export(.re2_named_capturing_groups)]]
SEXP re2_named_capturing_groups(SEXP pattern) {

  re2::RE2Proxy container(pattern); 
  List result(1);

  const std::map<std::string, int>& groups
    = container[0].get().NamedCapturingGroups();
  if (groups.size() > 0) {
    std::vector<std::string> keys;
    std::vector<int> values;
    values.reserve(groups.size());
    keys.reserve(groups.size());
    for (auto const& element : groups) {
      keys.push_back(element.first);
      values.push_back(element.second);
    }
    IntegerVector groupids = wrap(values);
    groupids.attr("names") = keys;
    result[0] = groupids;
  }

  return result(0);
}
