// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"
#include "re2_do_match.h"

using namespace Rcpp;

namespace {
  struct DoMatchM : re2::DoMatchIntf {
    StringMatrix &result;
    int count = 1;
    re2::RE2Proxy &re2proxy;
    DoMatchM(StringMatrix &r, re2::RE2Proxy &re2proxy)
      : result(r), re2proxy(re2proxy) {}
    bool proceed() { return count-- > 0 ? true : false; }
    void match_found(int i,
		     re2::StringPiece &text,
		     re2::RE2Proxy::Adapter &re2,
		     const re2::AllMatches &all_matches) {
      count = 1;
      re2::StringPiece *sp_arr = all_matches.at(0);

      if (re2proxy.size() == 1) {
	for (int col = 0; col < re2.nsubmatch(); col++) {
	  result(i, col) = sp_arr[col].data() == NULL
	    ? NA_STRING : String(sp_arr[col].as_string());
	}
	return; 
      }
      std::vector<bool> found(re2proxy.all_groups_count(), false);
      for (int col = 0; col < re2.nsubmatch(); col++) {
	std::string &col_name = re2.group_names().at(col);
	std::vector<std::string> &all = re2proxy.all_group_names();
	auto it = std::lower_bound(all.begin(), all.end(), col_name);
	if (it == all.end() || *it != col_name) {
	  const char* fmt
	    = "Error: group names mismatch.";
	  throw ::Rcpp::not_compatible(fmt);
	}
	std::size_t index
	  = std::distance(all.begin(), it);
	result(i, index) = sp_arr[col].data() == NULL
	  ? NA_STRING : String(sp_arr[col].as_string());
	found[index] = true;
      }
      for (int col = 0; col < re2proxy.all_groups_count(); col++) {
	if (!found[col]) {
	  result(i, col) = NA_STRING;
	}
      }      
    }
    void match_not_found(int i,
			 SEXP text,
			 re2::RE2Proxy::Adapter &re2) {
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
    void match_found(int i,
		     re2::StringPiece &text,
		     re2::RE2Proxy::Adapter &re2,
		     const re2::AllMatches &all_matches) {
      count = 1;
      StringMatrix mat(1, re2.nsubmatch());
      colnames(mat) = wrap(re2.group_names());
      re2::StringPiece *sp_arr = all_matches.at(0);
      for (int col = 0; col < re2.nsubmatch(); col++) {	    
	mat(0, col) = sp_arr[col].data() == NULL
	  ? NA_STRING : String(sp_arr[col].as_string());
      }
      result[i] = mat;
    }
    void match_not_found(int i,
			 SEXP text,
			 re2::RE2Proxy::Adapter &re2) {
      count = 1;
      StringMatrix mat(0, re2.nsubmatch());
      colnames(mat) = wrap(re2.group_names());
      result[i] = mat;
    }
    SEXP get() { return result; }
  };
}

//' Extract matched groups from a string
//'
//' @description
//' Vectorized over string. Match against a string using a regular
//'    expression and extract matched substrings.

//' XXX re2_match_all extracts all matching
//'
//' Matching regexp "(foo)|(bar)baz" on "barbazbla" will return
//'   submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
//'   the entire matching text. '.1' is the first group,
//'   and so on. Groups can also be named.
//'
//' @inheritParams re2_match_cpp
//'
//' @return A character matrix mapping group names to matching
//'   substrings. First column is the entire matching text, followed
//'   by one column for each capture group. 
//'
//' @usage
//'   re2_match(string, pattern)
//'   re2_match(string, re2_regexp(pattern, ...))
//'
//' @examples
//' 
//' ## Substring extraction
//' s <- c("barbazbla", "foobar", "this is a test")
//' pat <- "(foo)|(?P<TestGroup>bar)baz"
//' re2_match(s, pat)
//' #
//' res <- re2_match(s, pat)
//' stopifnot(is.matrix(res))
//' stopifnot(colnames(res) == c(".0", ".1", "TestGroup"))
//' stopifnot(nrow(res) == 3, ncol(res) == 3)
//' stopifnot(res[2, 1:2] == c("foo", "foo"))
//' 
//' ## Compile regexp
//' re <- re2_regexp("(foo)|(BaR)baz", case_sensitive=FALSE)
//' re2_match(s, re)
//' 
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax, and 
//'   \code{\link{re2_match_cpp}} which this function wraps.
//'   \code{\link{re2_match_all}} to extract all matching substrings,
//'      not just the first. 
//'
// [[Rcpp::export]]
SEXP re2_match(StringVector string, SEXP pattern,
		       bool simplify = true) {
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
    void match_found(int i,
		     re2::StringPiece &text,
		     re2::RE2Proxy::Adapter &re2,
		     const re2::AllMatches &all_matches) {
      StringMatrix mat(all_matches.size(), re2.nsubmatch());
      colnames(mat) = wrap(re2.group_names());
      for (int row = 0; row < all_matches.size(); row++) {
	re2::StringPiece *sp_arr = all_matches.at(row);
	for (int col = 0; col < re2.nsubmatch(); col++) {	    
	  mat(row, col) = sp_arr[col].data() == NULL
	    ? NA_STRING : String(sp_arr[col].as_string());
	}
      }
      result[i] = mat;
    }
    void match_not_found(int i,
			 SEXP text,
			 re2::RE2Proxy::Adapter &re2) {
      StringMatrix mat(0, re2.nsubmatch());
      colnames(mat) = wrap(re2.group_names());
      result[i] = mat;
    }
    SEXP get() { return result; }
  };
}

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
    void match_found(int i,
		     re2::StringPiece &text,
		     re2::RE2Proxy::Adapter &re2,
		     const re2::AllMatches &all_matches) {
      result[i] = all_matches.size();
    }
    void match_not_found(int i,
			 SEXP text,
			 re2::RE2Proxy::Adapter &re2) {
      result[i] = 0;
    }
    SEXP get() { return result; }
  };
}

// [[Rcpp::export]]
IntegerVector re2_count(StringVector string, SEXP pattern) {
  IntegerVector result(string.size());
  DoCount doer(result);
  return re2_do_match(string, pattern, doer);
}
