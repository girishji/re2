// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"
#include "re2_do_match.h"

using namespace Rcpp;

SEXP re2_do_match(StringVector string, SEXP pattern,
		  re2::DoMatchIntf &doer) {
  re2::RE2Proxy re2proxy(pattern);
  return re2_do_match(string, re2proxy, doer);
}

SEXP re2_do_match(StringVector string, re2::RE2Proxy &re2proxy,
		  re2::DoMatchIntf &doer) {
  
  StringVector& vstring = string;
  if ((vstring.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
      "multiple of pattern vector length" << '\n';
  }
    
  for (int i = 0; i < vstring.size(); i++) {
    int re_idx = i % re2proxy.size();
    
    bool match_found = false;
    if (vstring(i) != NA_STRING) {
      re2::StringPiece text(R_CHAR(vstring(i)));
      std::vector<re2::StringPiece*> all_matches;

      int nsubmatch = re2proxy[re_idx].nsubmatch();

      size_t consumed;
      while (true) {
	if (!doer.proceed()) {
	  break;
	}
	re2::StringPiece *submatch = new re2::StringPiece[nsubmatch];
	if (re2proxy[re_idx].get().Match(text, 0, text.size(), RE2::UNANCHORED,
				 submatch, nsubmatch)) {
	  all_matches.push_back(submatch);
	  consumed = static_cast<size_t>(submatch[0].end() - text.begin());
	  if (consumed == 0) {
	    break;
	  }
	  text.remove_prefix(consumed);
	} else {
	  delete[] submatch;
	  break;
	}
      }

      if (!all_matches.empty()) {
	text.set(R_CHAR(vstring(i))); // reset
	doer.match_found(i, text, re2proxy[re_idx], all_matches);
	match_found = true;
	// delete heap vector
	for(const re2::StringPiece* sp_arr : all_matches) {
	  delete[] sp_arr;
	}
      }
    }

    if (vstring(i) == NA_STRING || !match_found) {
      doer.match_not_found(i, vstring(i), re2proxy[re_idx]);
    }
  }
  return doer.get();
}
