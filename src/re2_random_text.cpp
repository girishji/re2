// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include <ctime>
#include <cstdlib>

using namespace Rcpp;

// Benchmark: failed search for regexp in random text.
//
// Generate random text that won't contain the search string,
// to test worst-case search behavior.
//
//// [[Rcpp::export(.re2_random_text)]]
std::string re2_random_text(int64_t nbytes) {
  static const std::string* const text = []() {
    std::string* text = new std::string;
    srand(1);
    text->resize(16<<20);
    for (int64_t i = 0; i < 16<<20; i++) {
      // Generate a one-byte rune that isn't a control character (e.g. '\n').
      // Clipping to 0x20 introduces some bias, but we don't need uniformity.
      int byte = rand() & 0x7F;
      if (byte < 0x20)
        byte = 0x20;
      (*text)[i] = byte;
    }
    return text;
  }();
  if (nbytes > 16<<20) {
    const char* fmt
      = "Expecting nbytes <= 16<<20: [type=%d].";
    throw ::Rcpp::not_compatible(fmt, nbytes);
  }    
  return text->substr(0, nbytes);
}
