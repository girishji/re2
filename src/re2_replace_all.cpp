// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Global replace
//
// @description
// Replace successive non-overlapping occurrences of "pattern" in
//   "text" with "rewrite" string.
// \preformatted{
//   re2_global_replace("yabba dabba doo", "b+", "d")
// }
// will result in "yada dada doo".
// Replacements are not subject to re-matching.
//
// Because \verb{re2_global_replace} only replaces non-overlapping matches,
//   replacing "ana" within "banana" makes only one replacement, not
//   two.
//
// @return A character string or vector with replacements. An integer
//   count of number of replacements made may also be returned
//   depending on the options.
//
// @examples
// s <-  c("yabba dabba doo", "famab")
// res <- re2_global_replace(s, "b+", "d")
// expected <- c("yada dada doo", "famad")
// stopifnot(res == expected)
// 
// re <- "(qu|[b-df-hj-np-tv-z]*)([a-z]+)"
// rewrite <- "\\2\\1ay"
// s <- "the quick brown fox jumps over the lazy dogs."
// expected <- "ethay ickquay ownbray oxfay umpsjay overay ethay azylay ogsday."
// res <- re2_global_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "\\w+"
// rewrite <- "\\0-NOSPAM"
// s <- "abcd.efghi@google.com"
// expected <- "abcd-NOSPAM.efghi-NOSPAM@google-NOSPAM.com-NOSPAM"
// res <- re2_global_replace(s, re, rewrite)
// stopifnot(res == expected)
//  
// re <- "a.*a"
// rewrite <- "(\\0)"
// s <- "aba\naba"
// expected <- "(aba)\n(aba)"
// res <- re2_global_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "b"
// rewrite <- "bb"
// s <- "ababababab"
// expected <- "abbabbabbabbabb"
// res <- re2_global_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "b+"
// rewrite <- "bb"
// s <- "bbbbbb"
// expected <- "bb"
// res <- re2_global_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "b*"
// rewrite <- "bb"
// s <- c("bbbbbb", "aaaaa")
// expected <- c("bb", "bbabbabbabbabbabb") 
// res <- re2_global_replace(s, re, rewrite)
// stopifnot(res == expected)
//
// [[Rcpp::export]]
SEXP re2_replace_all(StringVector string, SEXP pattern,
		     std::string& rewrite,
		     bool count = false) {
  
  re2::RE2Proxy re2proxy(pattern);
  StringVector sv(string.size());
  IntegerVector cntv(string.size());

  if ((string.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
      "multiple of pattern vector length" << '\n';
  }
  for (int i = 0; i < string.size(); i++) {
    int re_idx = i % re2proxy.size();

    if (string(i) == NA_STRING) {
      sv[i] = NA_STRING;
      cntv[i] = NA_INTEGER;
      continue;
    }

    std::string str = as<std::string>(string(i));
    int cnt = RE2::GlobalReplace(&str, re2proxy[re_idx].get(), rewrite);
    sv[i] = str;
    if (count) {
      cntv[i] = cnt;
    }
  }

  return count ? cntv : sv;
}
