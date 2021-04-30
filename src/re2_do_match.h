// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#ifndef RE2_DO_MATCH_H_
#define RE2_DO_MATCH_H_

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

namespace re2 {
  typedef std::vector<re2::StringPiece*> AllMatches;
  typedef std::vector<std::string> StringVector;

  struct DoMatchIntf {
    virtual void match_found(int i,
			     re2::StringPiece &text,
			     re2::RE2Proxy::Adapter &re2,
			     const AllMatches &all_matches) = 0;
    virtual void match_not_found(int i,
				 SEXP text,
				 re2::RE2Proxy::Adapter &re2) = 0;
    virtual bool proceed() { return true; };
    virtual SEXP get() = 0;
  };
}

SEXP re2_do_match(StringVector string, SEXP pattern, re2::DoMatchIntf &doer);
SEXP re2_do_match(StringVector string, re2::RE2Proxy &re2proxy,
		  re2::DoMatchIntf &doer);

#endif
