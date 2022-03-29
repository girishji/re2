// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#ifndef RE2_RE2_PROXY_H_
#define RE2_RE2_PROXY_H_

#include <Rcpp.h>
#include <re2/re2.h>
#include <memory>

using namespace Rcpp;

namespace re2 {

struct RE2Proxy {

  struct Adapter {
    Adapter(const RE2 *re2p) : re2p(re2p) {}
    Adapter(const std::string &pattern) {
      re2p = new RE2(pattern);
      freeable = true;
      if (!(re2p->ok())) {
        throw std::invalid_argument(re2p->error());
      }
    }
    const RE2 &get() const { return *re2p; }
    std::vector<std::string> &group_names();
    int nsubmatch() {
      if (_nsubmatch < 0) {
        _nsubmatch = re2p->NumberOfCapturingGroups() + 1;
      }
      return _nsubmatch;
    }
    virtual ~Adapter() {
      if (freeable) {
        delete re2p;
      }
    }

  private:
    bool freeable = false;
    const RE2 *re2p;
    int _nsubmatch = -1;
    std::vector<std::string> _group_names;
    Adapter();
  };

  RE2Proxy(const SEXP &input);
  Adapter &operator[](int index) { return *container.at(index); }
  int size() { return container.size(); }

  std::vector<std::string> &all_group_names();
  int all_groups_count();

private:
  typedef std::unique_ptr<Adapter> RE2AdapterPtr;
  std::vector<RE2AdapterPtr> container;
  void append(Adapter *ap) { container.push_back(RE2AdapterPtr(ap)); };
  std::vector<std::string> _all_group_names;
  RE2Proxy();
};
} // namespace re2
#endif
