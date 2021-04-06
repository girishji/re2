// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2container.h"

using namespace Rcpp;

//' Capturing indices to group names
//'
//' Return a map (named character vector) from capturing indices 
//'   to names of groups (if named). The mapping has no
//'   entries for unnamed groups. If parameter is a vector of pattern
//'   strings, then a list of mappings is returned.
//'
//' @param pattern Character string containing a regular expression,
//'    or a precompiled regular expression (see \code{\link{re2_re2}}).
//'    Or, a vector of such pattern strings.
//'
//' @return A named character vector reflecting the mapping from
//'   capturing indices to names. Or, a list of mappings if parameter
//'   is a vector.
//'
//' @examples
//' cgn <- re2_capturing_group_names("((abc)(?P<G2>)|((e+)(?P<G2>.*)(?P<G1>u+)))")
//' # 1st group is the outer paranthesis, 2nd groups is the unamed (abc), and so on.
//' stopifnot(cgn["3"] == "G2")
//' stopifnot(cgn["6"] == "G2")
//' stopifnot(cgn["7"] == "G1")
//' stopifnot(length(cgn) == 3)
//' stopifnot(is.na(cgn["1"]))
//'
//' # Same as above, except using compiled pattern:
//' re2p <- re2_re2("((abc)(?P<G2>)|((e+)(?P<G2>.*)(?P<G1>u+)))")
//' cgn <- re2_capturing_group_names(re2p)
//'
//' # If input is a vector of patterns, a list is returned:
//' cgn <- re2_capturing_group_names(c("bar", "(foo)(?P<Gr1>)"))
//' stopifnot(mode(cgn) == "list")
//'
//' @seealso \code{\link{re2_named_capturing_groups}},
//'   \code{\link{re2_number_of_capturing_groups}},
//'   \code{\link{re2_re2}}, \code{\link{re2_replace}},
//'   \code{\link{re2_match}}, \code{\link{re2_global_replace}},
//'   \code{\link{re2_extract}}.
// [[Rcpp::export]]
SEXP re2_capturing_group_names(SEXP pattern) {

  re2::RE2Container container(pattern); // vectorize
  const std::vector<re2::RE2ProxyPtr> &rv = container.get();
  List result(rv.size());

  for (std::vector<re2::RE2ProxyPtr>::size_type i = 0;
       i < rv.size(); i++) {
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
