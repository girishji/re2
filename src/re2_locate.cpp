// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_do_match.h"
#include "re2_re2proxy.h"

using namespace Rcpp;

namespace {
struct DoLocate : re2::DoMatchIntf {
  IntegerMatrix &result;
  int count = 1;
  DoLocate(IntegerMatrix &r) : result(r) {
    std::vector<std::string> gnames = {"begin", "end"};
    colnames(result) = wrap(gnames);
  }
  bool proceed() { return count-- > 0 ? true : false; }
  void match_found(int i, re2::StringPiece &text, re2::RE2Proxy::Adapter &re2,
                   const re2::AllMatches &all_matches) {
    count = 1;
    re2::StringPiece *sp_arr = all_matches.at(0);
    if (sp_arr[0].data() == NULL) {
      result(i, 0) = NA_INTEGER;
      result(i, 1) = NA_INTEGER;
    } else {
      result(i, 0) = static_cast<size_t>(sp_arr[0].begin() - text.begin() + 1);
      result(i, 1) = static_cast<size_t>(sp_arr[0].end() - text.begin());
    }
  }
  void match_not_found(int i, SEXP text, re2::RE2Proxy::Adapter &re2) {
    count = 1;
    result(i, 0) = NA_INTEGER;
    result(i, 1) = NA_INTEGER;
  }
  SEXP get() { return result; }
};
} // namespace

//' Locate the start and end of pattern in a string
//'
//' @description
//' Vectorized over string and pattern. For matches of 0 length (ex.
//'   special patterns like "$") end will be one character greater than
//'   beginning.
//'
//' @inheritParams re2_match
//'
//' @return \code{re2_locate} returns an integer matrix, and
//'   \code{re2_locate_all} returns a list of integer matrices.
//'
//' @example inst/examples/locate.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax.
//'
// [[Rcpp::export]]
IntegerMatrix re2_locate(StringVector string, SEXP pattern) {
  IntegerMatrix result(string.size(), 2);
  DoLocate doer(result);
  return re2_do_match(string, pattern, doer);
}

namespace {
struct DoLocateAll : re2::DoMatchIntf {
  List &result;
  DoLocateAll(List &r) : result(r) {}
  void match_found(int i, re2::StringPiece &text, re2::RE2Proxy::Adapter &re2,
                   const re2::AllMatches &all_matches) {
    IntegerMatrix mat(all_matches.size(), 2);
    std::vector<std::string> gnames = {"begin", "end"};
    colnames(mat) = wrap(gnames);
    for (int row = 0; row < all_matches.size(); row++) {
      re2::StringPiece *sp_arr = all_matches.at(row);
      if (sp_arr[0].data() == NULL) {
        mat(row, 0) = NA_INTEGER;
        mat(row, 1) = NA_INTEGER;
      } else {
        mat(row, 0) = static_cast<size_t>(sp_arr[0].begin() - text.begin() + 1);
        mat(row, 1) = static_cast<size_t>(sp_arr[0].end() - text.begin());
      }
    }
    result[i] = mat;
  }
  void match_not_found(int i, SEXP text, re2::RE2Proxy::Adapter &re2) {
    IntegerMatrix mat(0, 2);
    std::vector<std::string> gnames = {"begin", "end"};
    colnames(mat) = wrap(gnames);
    result[i] = mat;
  }
  SEXP get() { return result; }
};
} // namespace

//' @inherit re2_locate
// [[Rcpp::export]]
List re2_locate_all(StringVector string, SEXP pattern) {
  List result(string.size());
  DoLocateAll doer(result);
  return re2_do_match(string, pattern, doer);
}
