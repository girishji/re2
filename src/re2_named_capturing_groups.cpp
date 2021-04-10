// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2container.h"

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
// @seealso \code{\link{re2_capturing_group_names}},
//   \code{\link{re2_number_of_capturing_groups}},
//   \code{\link{re2_re2}}, \code{\link{re2_replace}},
//   \code{\link{re2_match}}, \code{\link{re2_global_replace}},
//   \code{\link{re2_extract}}.
// [[Rcpp::export(.re2_named_capturing_groups)]]
SEXP re2_named_capturing_groups(SEXP pattern) {

  re2::RE2Container container(pattern); // vectorize
  const std::vector<re2::RE2ProxyPtr> &rv = container.get();
  List result(rv.size());

  for (std::vector<re2::RE2ProxyPtr>::size_type i = 0;
       i < rv.size(); i++) {
    const std::map<std::string, int>& groups
      = rv[i]->get().NamedCapturingGroups();
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
      result[i] = groupids;
    }
  }

  if (result.size() == 1) {
    return result(0);
  } else {
    return result;
  }
}
