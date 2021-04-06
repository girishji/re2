// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2container.h"

using namespace Rcpp;

//' Number of capturing subpatterns
//'
//' Return the number of capturing subpatterns, or -1 if the
//' regexp wasn't valid on construction.  The overall match ($0)
//' does not count: if the regexp is "(a)(b)", returns 2.
//'
//' @inheritParams re2_capturing_group_names
//'
//' @return Number of capturing groups or -1 on error.
//'
//' @examples
//' s <- "(?P<A>expr(?P<B>expr)(?P<C>expr))((expr)(?P<D>expr))"
//' stopifnot(re2_number_of_capturing_groups(s) == 6)
//' #
//' re2p <- re2_re2("directions from (?P<S>.*) to (?P<D>.*)")
//' stopifnot(re2_number_of_capturing_groups(re2p) == 2)
//'
//' @seealso
//'   \code{\link{re2_named_capturing_groups}},
//'   \code{\link{re2_capturing_group_names}},
//'   \code{\link{re2_re2}}, \code{\link{re2_replace}},
//'   \code{\link{re2_match}}, \code{\link{re2_global_replace}},
//'   \code{\link{re2_extract}}.
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
