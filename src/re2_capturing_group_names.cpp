// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Capturing indices to group names
//
// Return a map (named character vector) from capturing indices 
//   to names of groups (if named). The mapping has no
//   entries for unnamed groups. If parameter is a vector of pattern
//   strings, then a list of mappings is returned.
//
// @param pattern Character string containing a regular expression,
//    or a precompiled regular expression (see \code{\link{re2_re2}}).
//    Or, a vector of such pattern strings.
//
// @return A named character vector reflecting the mapping from
//   capturing indices to names. Or, a list of mappings if parameter
//   is a vector.
//
// @examples
// cgn <- re2_capturing_group_names("((abc)(?P<G2>)|((e+)(?P<G2>.*)(?P<G1>u+)))")
// # 1st group is the outer paranthesis, 2nd groups is the unamed (abc), and so on.
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

  const std::map<int, std::string>& groups
    = container[0].get().CapturingGroupNames();
  if (groups.size() > 0) {
    std::vector<std::string> values;
    std::vector<int> keys;
    values.reserve(groups.size());
    keys.reserve(groups.size());
    for (auto const& element : groups) {
      keys.push_back(element.first);
      values.push_back(element.second);
    }
    StringVector groupids = wrap(values);
    groupids.attr("names") = keys;
    result[0] = groupids;
  }

  return result[0];
}
