#include "re2_re2container.h"

using namespace Rcpp;

namespace re2 {

  RE2Container::RE2Container(const SEXP regex,
			     Nullable<List> more_options) {
      
    if (TYPEOF(regex) == STRSXP || TYPEOF(regex) == VECSXP) {
      List regexs = as<List>(regex);
      container.reserve(regexs.size());
      for (List::iterator it = regexs.begin();
	   it != regexs.end(); ++it) {
	SEXP deref = static_cast<SEXP>(*it);
	if (TYPEOF(deref) == STRSXP && deref == NA_STRING) {
	  continue;
	}
	RE2ProxyPtr re2ptr(new RE2Proxy(deref));
	container.push_back(std::move(re2ptr));  
      }
    } else if (TYPEOF(regex) == EXTPTRSXP) {
	RE2ProxyPtr re2ptr(new RE2Proxy(regex));
	container.push_back(std::move(re2ptr));  
    } else {
      const char* fmt
	= "Expecting a re2 pointer or string: [type=%s].";
      throw ::Rcpp::not_compatible(fmt, Rf_type2char(TYPEOF(regex)));
    }
  }
}
