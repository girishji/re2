// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2container.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_capturing_group_names(SEXP pattern) {

  re2::RE2Container container(pattern); // vectorize
  const std::vector<re2::RE2ProxyPtr> &rv = container.get();
  List result(rv.size());

  for (int i = 0; i < rv.size(); i++) {
    const std::map<int, std::string>& groups
      = rv[i]->get().CapturingGroupNames();
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
      result[i] = groupids;
    }
  }

  if (result.size() == 1) {
    return result[0];
  } else {
    return result;
  }
}
