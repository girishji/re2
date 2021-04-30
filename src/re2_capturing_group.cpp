// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

// Capturing indices to group names
//
// Return a map (named character vector) from capturing indices
//   to names of groups (if named). The mapping has no
//   entries for unnamed groups. If parameter is a vector of pattern
//   strings, then a list of mappings is returned.
//
// @param pattern Character string containing a regular expression.
//
// @return A named character vector reflecting the mapping from
//   capturing indices to names. Or, a list of mappings if parameter
//   is a vector.
//
// @examples
// cgn <-
// re2_capturing_group_names("((abc)(?P<G2>)|((e+)(?P<G2>.*)(?P<G1>u+)))") # 1st
// group is the outer paranthesis, 2nd groups is the unamed (abc), and so on.
// stopifnot(cgn["3"] == "G2")
// stopifnot(cgn["6"] == "G2")
// stopifnot(cgn["7"] == "G1")
// stopifnot(length(cgn) == 3)
// stopifnot(is.na(cgn["1"]))
//
// [[Rcpp::export(.re2_capturing_group_names)]]
SEXP re2_capturing_group_names(SEXP pattern) {

  re2::RE2Proxy container(pattern);
  List result(1);

  const std::map<int, std::string> &groups =
      container[0].get().CapturingGroupNames();
  if (groups.size() > 0) {
    std::vector<std::string> values;
    std::vector<int> keys;
    values.reserve(groups.size());
    keys.reserve(groups.size());
    for (auto const &element : groups) {
      keys.push_back(element.first);
      values.push_back(element.second);
    }
    StringVector groupids = wrap(values);
    groupids.attr("names") = keys;
    result[0] = groupids;
  }

  return result[0];
}

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

  const std::map<std::string, int> &groups =
      container[0].get().NamedCapturingGroups();
  if (groups.size() > 0) {
    std::vector<std::string> keys;
    std::vector<int> values;
    values.reserve(groups.size());
    keys.reserve(groups.size());
    for (auto const &element : groups) {
      keys.push_back(element.first);
      values.push_back(element.second);
    }
    IntegerVector groupids = wrap(values);
    groupids.attr("names") = keys;
    result[0] = groupids;
  }

  return result(0);
}

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
