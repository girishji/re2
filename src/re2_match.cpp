// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_do_match.h"
#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

namespace {
struct DoMatchM : re2::DoMatchIntf {
  StringMatrix &result;
  int count = 1;
  re2::RE2Proxy &re2proxy;
  DoMatchM(StringMatrix &r, re2::RE2Proxy &re2proxy)
      : result(r), re2proxy(re2proxy) {}
  bool proceed() { return count-- > 0 ? true : false; }
  void match_found(int i, re2::StringPiece &text, re2::RE2Proxy::Adapter &re2,
                   const re2::AllMatches &all_matches) {
    count = 1;
    re2::StringPiece *sp_arr = all_matches.at(0);

    if (re2proxy.size() == 1) {
      for (int col = 0; col < re2.nsubmatch(); col++) {
        result(i, col) = sp_arr[col].data() == NULL
                             ? NA_STRING
                             : String(sp_arr[col].as_string());
      }
      return;
    }
    std::vector<bool> found(re2proxy.all_groups_count(), false);
    for (int col = 0; col < re2.nsubmatch(); col++) {
      std::string &col_name = re2.group_names().at(col);
      std::vector<std::string> &all = re2proxy.all_group_names();
      auto it = std::lower_bound(all.begin(), all.end(), col_name);
      if (it == all.end() || *it != col_name) {
        const char *fmt = "Error: group names mismatch.";
        throw ::Rcpp::not_compatible(fmt);
      }
      std::size_t index = std::distance(all.begin(), it);
      result(i, index) = sp_arr[col].data() == NULL
                             ? NA_STRING
                             : String(sp_arr[col].as_string());
      found[index] = true;
    }
    for (int col = 0; col < re2proxy.all_groups_count(); col++) {
      if (!found[col]) {
        result(i, col) = NA_STRING;
      }
    }
  }
  void match_not_found(int i, SEXP text, re2::RE2Proxy::Adapter &re2) {
    count = 1;
    for (int dcol = 0; dcol < re2proxy.all_groups_count(); dcol++) {
      result(i, dcol) = NA_STRING;
    }
  }
  SEXP get() { return result; }
};

struct DoMatchL : re2::DoMatchIntf {
  List &result;
  int count = 1;
  DoMatchL(List &r) : result(r) {}
  bool proceed() { return count-- > 0 ? true : false; }
  void match_found(int i, re2::StringPiece &text, re2::RE2Proxy::Adapter &re2,
                   const re2::AllMatches &all_matches) {
    count = 1;
    StringVector vect(re2.nsubmatch());
    vect.names() = wrap(re2.group_names());
    re2::StringPiece *sp_arr = all_matches.at(0);
    for (int col = 0; col < re2.nsubmatch(); col++) {
      vect[col] = sp_arr[col].data() == NULL ? NA_STRING
                                             : String(sp_arr[col].as_string());
    }
    result[i] = vect;
  }
  void match_not_found(int i, SEXP text, re2::RE2Proxy::Adapter &re2) {
    count = 1;
    StringVector vect(re2.nsubmatch());
    vect.names() = wrap(re2.group_names());
    result[i] = vect;
  }
  SEXP get() { return result; }
};
} // namespace

//' Extract matched groups from a string
//'
//' @description
//' Vectorized over string and pattern. Match against a string using a regular
//'    expression and extract matched substrings. \code{re2_match} extracts
//'    first matched substring, and \code{re2_match_all} extracts all matches.
//'
//' Matching regexp "(foo)|(bar)baz" on "barbazbla" will return
//'   submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
//'   the entire matching text. '.1' is the first group,
//'   and so on. Groups can also be named.
//'
//' @param string A character vector, or an object which can be coerced to one.
//' @param pattern Character string containing a regular expression,
//'    or a pre-compiled regular expression, or a (mixed) vector of character
//'    strings and pre-compiled regular expressions. \cr
//'   See \code{\link{re2_regexp}} for available options. \cr
//'   See \link{re2_syntax} for RE2 syntax. \cr
//' @param simplify If TRUE, the default, returns a character matrix. If FALSE,
//'   returns a list. Not applicable to \code{re2_match_all}.
//'
//' @return In case of \code{re2_match} a character matrix. First column is the
//'    entire matching text, followed by one column for each capture group. If
//'    simplify is FALSE, returns a list of named character vectors. \cr
//'    In case of \code{re2_match_all} a list of character matrices.
//'
//' @example inst/examples/match.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax.
//'
// [[Rcpp::export]]
SEXP re2_match(StringVector string, SEXP pattern, bool simplify = true) {
  if (simplify) {
    re2::RE2Proxy re2proxy(pattern);
    StringMatrix result(string.size(), re2proxy.all_groups_count());
    colnames(result) = wrap(re2proxy.all_group_names());
    DoMatchM doer(result, re2proxy);
    return re2_do_match(string, re2proxy, doer);
  } else {
    List result(string.size());
    DoMatchL doer(result);
    return re2_do_match(string, pattern, doer);
  }
}

namespace {
struct DoMatchAll : re2::DoMatchIntf {
  List &result;
  DoMatchAll(List &r) : result(r) {}
  void match_found(int i, re2::StringPiece &text, re2::RE2Proxy::Adapter &re2,
                   const re2::AllMatches &all_matches) {
    StringMatrix mat(all_matches.size(), re2.nsubmatch());
    colnames(mat) = wrap(re2.group_names());
    for (int row = 0; row < all_matches.size(); row++) {
      re2::StringPiece *sp_arr = all_matches.at(row);
      for (int col = 0; col < re2.nsubmatch(); col++) {
        mat(row, col) = sp_arr[col].data() == NULL
                            ? NA_STRING
                            : String(sp_arr[col].as_string());
      }
    }
    result[i] = mat;
  }
  void match_not_found(int i, SEXP text, re2::RE2Proxy::Adapter &re2) {
    StringMatrix mat(0, re2.nsubmatch());
    colnames(mat) = wrap(re2.group_names());
    result[i] = mat;
  }
  SEXP get() { return result; }
};
} // namespace

//' @inherit re2_match
// [[Rcpp::export]]
List re2_match_all(StringVector string, SEXP pattern) {
  List result(string.size());
  DoMatchAll doer(result);
  return re2_do_match(string, pattern, doer);
}

namespace {
struct DoCount : re2::DoMatchIntf {
  IntegerVector &result;
  DoCount(IntegerVector &r) : result(r) {}
  void match_found(int i, re2::StringPiece &text, re2::RE2Proxy::Adapter &re2,
                   const re2::AllMatches &all_matches) {
    result[i] = all_matches.size();
  }
  void match_not_found(int i, SEXP text, re2::RE2Proxy::Adapter &re2) {
    result[i] = 0;
  }
  SEXP get() { return result; }
};
} // namespace

//' Count the number of matches in a string
//'
//' @description
//' Vectorized over string and pattern. Match against a string using a regular
//'    expression and return the count of matches.
//'
//' @inheritParams re2_match
//'
//' @return An integer vector.
//'
//' @example inst/examples/count.R
//'
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax.
//'
// [[Rcpp::export]]
IntegerVector re2_count(StringVector string, SEXP pattern) {
  IntegerVector result(string.size());
  DoCount doer(result);
  return re2_do_match(string, pattern, doer);
}
