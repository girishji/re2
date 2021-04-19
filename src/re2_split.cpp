// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"
#include "re2_do_match.h"

using namespace Rcpp;

namespace {
  struct DoSplit : re2::DoMatchIntf {
    List &result;
    int n;
    int counter;
    bool _n = false;
    DoSplit(List &r) : result(r) {}
    DoSplit(List &r, int n) : result(r), n(n), counter(n), _n(true)  {}
    bool proceed() {
      if (!_n) return true;
      return counter-- > 0 ? true : false;
    }
    void match_found(int i,
		     re2::StringPiece &text,
		     re2::RE2Proxy::Adapter &re2,
		     const re2::AllMatches &all_matches) {
      counter = n;
      if (all_matches.size() == 1) {
	re2::StringPiece *sp_arr = all_matches.at(0);
	if (sp_arr[0].size() == 0) {
	  result[i] = String(text.as_string());
	  return;
	}
      }
      StringVector splitted(all_matches.size() + 1);
      int row = 0;
      for ( ; row < all_matches.size(); row++) {
	re2::StringPiece *sp_arr = all_matches.at(row);
	size_t size = static_cast<size_t>(sp_arr[0].begin()
					  - text.begin());
	splitted[row] = String(std::string(text.begin(), size));
	text.remove_prefix(size + sp_arr[0].size());
      }
      splitted[row] = String(text.as_string());
      result[i] = splitted;
    }
    void match_not_found(int i,
			 SEXP text,
			 re2::RE2Proxy::Adapter &re2) {
      counter = n;
      result[i] = String(text);
    }
    SEXP get() { return result; }
  };
}

// [[Rcpp::export]]
SEXP re2_split(StringVector string, SEXP pattern,
	       bool simplify = false, double n = R_PosInf) {
  if (simplify) {
    List lst = re2_split(string, pattern, false, n);
    int maxcols = 0;
    for (int i = 0; i < lst.size(); i++) {
      StringVector sv(lst(i));
      if (sv.size() > maxcols) {
	maxcols = sv.size();
      }
    }
    StringMatrix result(string.size(), maxcols);
    for (int row = 0; row < lst.size(); row++) {
      StringVector sv(lst(row));
      int sv_size = sv.size();
      for (int col = 0; col < maxcols; col++) {
	result(row, col) = col < sv_size ? String(sv(col)) : NA_STRING;
      }
    }
    return result;    
  } else {
    List result(string.size());
    if (n == R_PosInf || n < 0) {
      DoSplit doer(result);
      return re2_do_match(string, pattern, doer);
    } else {
      DoSplit doer(result, std::round(n - 1));
      return re2_do_match(string, pattern, doer);
    }
  }  
}
