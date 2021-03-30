// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// [[Rcpp::export]]
SEXP re2_match(StringVector x, SEXP pattern,
	       Nullable<List> more_options = R_NilValue) {
  
  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(x.size());

  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  bool testing = false;
  re2::RE2Proxy::set_option(testing, "testing", more_options);
  
  RE2::Anchor anchor = RE2::UNANCHORED;;
  re2::RE2Proxy::set_option_anchor(anchor, "re_anchor", more_options);
  
  int nsubmatch = re2proxy.get().NumberOfCapturingGroups() + 1;
  re2::RE2Proxy::set_option_int(nsubmatch, "nsubmatch", more_options);
  
  std::vector<std::string> group_names(nsubmatch);
  if (nsubmatch > 0) {
    group_names[0] = ".0";
    const std::map<int, std::string>& cgroups
      = re2proxy.get().CapturingGroupNames();
    for (int i = 1; i < nsubmatch; i++) {
      auto search = cgroups.find(i);
      group_names[i] = search != cgroups.end()
	? search->second : "." + std::to_string(i);
    }
  }

  StringMatrix res(x.size(), nsubmatch);
  if (nsubmatch > 0 && !testing) {
    colnames(res) = wrap(group_names);
  }
  size_t startpos = 0;
  re2::RE2Proxy::set_option_uint(startpos, "startpos", more_options);

  
  // Maybe stack allocation is ok here. Just in case we have
  //  a lot of groups -- use heap allocation.
  std::unique_ptr<re2::StringPiece[]> submatch
    = std::unique_ptr<re2::StringPiece[]>(new re2::StringPiece[nsubmatch]);
  // re2::StringPiece submatch[nsubmatch];

  for (int i = 0; i < x.size(); i++) {
    
    if (x(i) == NA_STRING) {
      for (int j = 0; j < nsubmatch; j++) {
        res(i, j) = NA_STRING;
      }
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strp(R_CHAR(x(i))); // shallow copy (providing char *)
    size_t endpos = strp.size();
    re2::RE2Proxy::set_option_uint(endpos, "endpos", more_options);
    
    if (nsubmatch == 0) {
      lv[i] = re2proxy.get().Match(strp, startpos, endpos, anchor,
				   nullptr, 0);
    } else {
      lv[i] = re2proxy.get().Match(strp, startpos, endpos, anchor,
				   submatch.get(), nsubmatch);
				   
      for (int j = 0; j < nsubmatch; j++) {
        res(i, j) = submatch[j].data() == NULL
	  ? NA_STRING : String(submatch[j].as_string());
      }
    }
    // clear
    for (int sm = 0; sm < nsubmatch; sm++) {
      submatch[sm] = re2::StringPiece(); 
    }
  } // for
  
  if (nsubmatch == 0) {
    return lv;
  }
  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = res);
  }
  return logical ? lv : res;
}
