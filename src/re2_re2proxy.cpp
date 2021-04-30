
// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"

using namespace Rcpp;

namespace re2 {

RE2Proxy::RE2Proxy(const SEXP &input) {

  std::function<void(SEXP)> dfs; // recursively traverse list
  dfs = [this, &dfs](SEXP input) -> void {
    switch (TYPEOF(input)) {
    case EXTPTRSXP: {
      XPtr<RE2> xptr(input);
      append(new Adapter(xptr.checked_get()));
      break;
    }
    case STRSXP: {
      StringVector sv(input);
      for (int i = 0; i < sv.size(); i++) {
        append(new Adapter(as<std::string>(sv(i))));
      }
      break;
    }
    case VECSXP: {
      List alist(input);
      for (int i = 0; i < alist.size(); i++) {
        dfs(alist(i).get());
      }
      break;
    }
    default: {
      const char *fmt = "Expecting external pointer or string: [type=%s].";
      throw ::Rcpp::not_compatible(fmt, Rf_type2char(TYPEOF(input)));
    }
    }
  };

  if (TYPEOF(input) == STRSXP || TYPEOF(input) == VECSXP) {
    container.reserve(XLENGTH(input));
  }
  dfs(input);
  if (container.empty()) {
    throw ::Rcpp::not_compatible("Invalid pattern");
  }
}

std::vector<std::string> &RE2Proxy::Adapter::group_names() {
  if (_group_names.empty()) {
    _group_names.reserve(nsubmatch());
    _group_names.push_back(".0");
    const std::map<int, std::string> &cgroups = re2p->CapturingGroupNames();
    for (int i = 1; i < nsubmatch(); i++) {
      auto search = cgroups.find(i);
      _group_names.push_back(search != cgroups.end() ? search->second
                                                     : "." + std::to_string(i));
    }
  }
  return _group_names;
}

std::vector<std::string> &RE2Proxy::all_group_names() {
  if (_all_group_names.empty()) {
    if (container.size() == 1) {
      _all_group_names = container.at(0)->group_names();
    } else {
      std::set<std::string> set;
      for (auto &re2p : container) {
        for (auto &gr : re2p->group_names()) {
          set.insert(gr);
        }
      }
      _all_group_names.reserve(set.size());
      std::copy(set.begin(), set.end(), std::back_inserter(_all_group_names));
      std::sort(_all_group_names.begin(), _all_group_names.end());
    }
  }
  return _all_group_names;
}

int RE2Proxy::all_groups_count() {
  if (_all_group_names.empty()) {
    all_group_names();
  }
  return _all_group_names.size();
}
} // namespace re2
