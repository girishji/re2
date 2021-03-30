// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#ifndef RE2_RE2_PROXY_H_
#define RE2_RE2_PROXY_H_

#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

namespace re2 {

  class RE2Proxy {
    
  public:
    RE2Proxy(const SEXP regex,
	     Nullable<List> more_options = R_NilValue);
    const RE2& get() const;
    static void modify_options(RE2::Options& opt,
			       Nullable<List> more_options);
    static void set_option(bool& opt, const std::string& name,
			   Nullable<List> options);
    static void set_option_uint(size_t& opt,
				const std::string& name,
				Nullable<List> options);
    static void set_option_int(int& opt,
			       const std::string& name,
			       Nullable<List> options);
    static void set_option_anchor(RE2::Anchor &opt,
				  const std::string& name,
				  Nullable<List> options);
    static bool is_logical_out(Nullable<List> options);
    static bool is_verbose_out(Nullable<List> options);
    static bool is_count_out(Nullable<List> options);
    
  private:
    RE2* re2ptr;
    std::unique_ptr<RE2> re2uptr;
    bool uptr = false;
    RE2Proxy();
    static bool get_output_choice(std::vector<std::string>& optstrs,
				  Nullable<List> options);
  };
}
#endif
