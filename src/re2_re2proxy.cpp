
// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"

using namespace Rcpp;

namespace re2 {

  RE2Proxy::RE2Proxy(const SEXP regex,
		     Nullable<List> more_options) {

    if (TYPEOF(regex) == EXTPTRSXP) {
      XPtr<RE2> xptr(regex);
      re2ptr = xptr.checked_get();
    } else if (TYPEOF(regex) == STRSXP) {
      StringVector sv = as<StringVector>(regex);
      if (sv.size() != 1) {
	const char* fmt
	  = "Expecting a single pattern string: [length=%d].";
	throw ::Rcpp::not_compatible(fmt, sv.size());	
      }
      if (more_options.isNotNull()){
	RE2::Options opts;
	modify_options(opts, more_options);
	re2uptr = std::unique_ptr<RE2>(new RE2(as<std::string>(sv),
					     opts));
      } else {
	re2uptr = std::unique_ptr<RE2>(new RE2(as<std::string>(sv)));
      }
      uptr = true;
    } else {
      const char* fmt
	= "Expecting an external pointer or string: [type=%s].";
      throw ::Rcpp::not_compatible(fmt, Rf_type2char(TYPEOF(regex)));	
    }
  }

  /************************************************************/
  const RE2& RE2Proxy::get() const {
    return uptr ? *re2uptr : *re2ptr;
  }

  /************************************************************/
  bool RE2Proxy::get_output_choice(std::vector<std::string>& optstrs,
				   Nullable<List> options) {
    if (options.isNotNull()) {
      List mopts(options);
      for (auto str : optstrs) {
	if (mopts.containsElementNamed(str.c_str())) {
	  return as<bool>(mopts[str]);
	}
      }
    }
    return false;
  }

  /************************************************************/
  bool RE2Proxy::is_logical_out(Nullable<List> options) {
    std::vector<std::string> v{"logical", "l"};
    return get_output_choice(v, options);
  }

  /************************************************************/
  bool RE2Proxy::is_verbose_out(Nullable<List> options) {
    std::vector<std::string> v{"verbose", "v"};
    return get_output_choice(v, options);
  }

  /************************************************************/
  bool RE2Proxy::is_count_out(Nullable<List> options) {
    std::vector<std::string> v{"count", "c"};
    return get_output_choice(v, options);
  }
  
  /************************************************************/
  bool RE2Proxy::set_option(bool& opt, const std::string& name,
			    Nullable<List> options) {
    if (options.isNotNull()) {
      List mopts(options);
      if (mopts.containsElementNamed(name.c_str())) {
	opt = as<bool>(mopts[name]);
	return true;
      }
    }
    return false;
  }

  /************************************************************/
  bool RE2Proxy::set_option_uint(size_t& opt,
				 const std::string& name,
				 Nullable<List> options) {
    if (options.isNotNull()) {
      List mopts(options); // casting to underlying type List
      if (mopts.containsElementNamed(name.c_str())) {
	opt = as<size_t>(mopts[name]);
	return true;
      }
    }
    return false;
  }

    /************************************************************/
  bool RE2Proxy::set_option_int(int& opt,
				const std::string& name,
				Nullable<List> options) {
    if (options.isNotNull()) {
      List mopts(options); // casting to underlying type List
      if (mopts.containsElementNamed(name.c_str())) {
	opt = as<int>(mopts[name]);
	return true;
      }
    }
    return false;
  }

  /************************************************************/
  bool RE2Proxy::set_option_anchor(RE2::Anchor& anchor,
				   const std::string& name,
				   Nullable<List> options) {
    if (options.isNotNull()) {
      List mopts(options);
      if (mopts.containsElementNamed(name.c_str())) {
	const std::string& opt = as<std::string>(mopts[name]);
	if (opt == "UNANCHORED") {
	  anchor = RE2::UNANCHORED;
	} else if (opt == "ANCHOR_START") {
	  anchor = RE2::ANCHOR_START;
	} else if (opt == "ANCHOR_BOTH") {
	  anchor = RE2::ANCHOR_BOTH;
	} else {
	  const char* fmt
	    = "Expecting valid anchor type: [type=%s].";
	  throw ::Rcpp::not_compatible(fmt, opt);
	}
	return true;
      }
    }
    return false;
  }

  /************************************************************/
  void RE2Proxy::modify_options(RE2::Options& opt,
				Nullable<List> more_options) {
  
    opt.set_log_errors(false); // make 'quiet' option the default

    static auto encoding_enum = [] (std::string const& val) {
      return (val == "EncodingLatin1" || val == "Latin1")
	? RE2::Options::EncodingLatin1 : RE2::Options::EncodingUTF8;
    };
  
#define SETTER(name) else if (strcmp(R_CHAR(names(i)), #name) == 0) {	\
      opt.set_##name(as<bool>(mopts(i))); }
    
    if (more_options.isNotNull()) {
      List mopts(more_options);
      if (mopts.size() > 0) {
	StringVector names = mopts.names();

        for (int i = 0; i < names.size(); i++) {
	  if (strcmp(R_CHAR(names(i)), "encoding") == 0) {
	    opt.set_encoding(encoding_enum(as<std::string>(mopts(i))));
	  }
  	
	  SETTER(posix_syntax)
          SETTER(longest_match)
          SETTER(log_errors)
          SETTER(literal)
          SETTER(never_nl)
          SETTER(dot_nl)
          SETTER(never_capture)
          SETTER(case_sensitive)
          SETTER(perl_classes)
          SETTER(word_boundary)
          SETTER(one_line)
  
	  else if (strcmp(R_CHAR(names(i)), "max_mem") == 0) {
            opt.set_max_mem(as<int>(mopts(i)));
	  } else {
	    const char* fmt
	      = "Expecting valid option: [type=%s].";
	    throw ::Rcpp::not_compatible(fmt, R_CHAR(names(i)));
	  }
        }
      }
    }
  }
}
