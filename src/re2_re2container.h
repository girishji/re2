// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE

#ifndef RE2_RE2_CONTAINER_H_
#define RE2_RE2_CONTAINER_H_

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

namespace re2 {

  typedef std::unique_ptr<RE2Proxy> RE2ProxyPtr;
  
  class RE2Container {
    
  public:
    RE2Container(const SEXP regex,
		 Nullable<List> more_options = R_NilValue);
    const std::vector<RE2ProxyPtr>& get() const { return container; };

  private:
    std::vector<RE2ProxyPtr> container;
    RE2Container();
  };
}
#endif
