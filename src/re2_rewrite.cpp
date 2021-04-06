// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;


// Append the "rewrite" string, with backslash subsitutions from "vec",
// and return the resulting string or TRUE/FALSE depending on options.
// This method can fail because of a malformed
// rewrite string.  CheckRewriteString guarantees that the rewrite will
// be sucessful.
//
// Not exporting this method since it seems redundant.
// // [[Rcpp::export]]
SEXP re2_rewrite(SEXP pattern,
		 StringVector rewrite,
		 StringVector vec,
		 int veclen,
		 Nullable<List> more_options = R_NilValue) {

  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(rewrite.size());
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  StringVector res(rewrite.size());

  std::unique_ptr<re2::StringPiece[]> vec_sp
    = std::unique_ptr<re2::StringPiece[]>(new re2::StringPiece[vec.size()]);  
  for (int i = 0; i < vec.size(); i++) {
    vec_sp[i] = re2::StringPiece(R_CHAR(vec(i)));
  }
  
  for (int i = 0; i < rewrite.size(); i++) {

    if (rewrite(i) == NA_STRING) {
      res[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strpc(R_CHAR(rewrite(i)));
    std::string out;
    lv[i] = re2proxy.get().Rewrite(&out, strpc, vec_sp.get(), veclen);
    res[i] = out;
  }

  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = res);
  }
  return logical ? lv : res;
}
