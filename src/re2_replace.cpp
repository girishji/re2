// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

// Replace the first match
//
// @description
// Replace the first match of "pattern" in "text" with
//   "rewrite" string. 
// \preformatted{
//   re2_replace("yabba dabba doo", "b+", "d")
// }
// will result in "yada dabba doo".
//
// @param text A character string or a character vector where
//   replacements are sought.
//
// @param rewrite Rewrite string. Backslash-escaped
//   digits (\\1 to \\9) can be used to insert text matching
//   corresponding parenthesized group from the pattern. \\0
//   refers to the entire matching text.
//
// @return A character string or vector with replacements. A logical
//   TRUE/FALSE vector may also be returned depending on the
//   options.
//
// @examples
// s <-  c("yabba dabba doo", "famab")
// res <- re2_replace(s, "b+", "d")
// expected <- c("yada dabba doo", "famad")
// stopifnot(res == expected)
// 
// res <- re2_replace("boris@kremvax.ru",
//                    "(.*)@([^.]*)", "\\2!\\1")
// expected <- "kremvax!boris.ru"
// stopifnot(res == expected)
// 
// re <- "(qu|[b-df-hj-np-tv-z]*)([a-z]+)"
// rewrite <- "\\2\\1ay"
// s <- "the quick brown fox jumps over the lazy dogs."
// expected <- "ethay quick brown fox jumps over the lazy dogs."
// res <- re2_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "\\w+"
// rewrite <- "\\0-NOSPAM"
// s <- "abcd.efghi@google.com"
// expected <- "abcd-NOSPAM.efghi@google.com"
// res <- re2_replace(s, re, rewrite)
// stopifnot(res == expected)
//   
// re <- "a.*a"
// rewrite <- "(\\0)"
// s <- "aba\naba"
// expected <- "(aba)\naba"
// res <- re2_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "b"
// rewrite <- "bb"
// s <- "ababababab"
// expected <- "abbabababab"
// res <- re2_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "b+"
// rewrite <- "bb"
// s <- "bbbbbb"
// expected <- "bb"
// res <- re2_replace(s, re, rewrite)
// stopifnot(res == expected)
// 
// re <- "b*"
// rewrite <- "bb"
// s <- c("bbbbbb", "aaaaa")
// expected <- c("bb", "bbaaaaa")
// res <- re2_replace(s, re, rewrite)
// stopifnot(res == expected)
//      
// [[Rcpp::export]]
SEXP re2_replace(StringVector string, SEXP pattern,
		 std::string& rewrite, bool logical=false) {
  
  re2::RE2Proxy re2proxy(pattern);
  CharacterVector cv(string.size());
  LogicalVector lv(string.size());

  if ((string.size() % re2proxy.size()) != 0) {
    Rcerr << "Warning: string vector length is not a "
      "multiple of pattern vector length" << '\n';
  }
  for (int i = 0; i < string.size(); i++) {
    int re_idx = i % re2proxy.size();

    if (string(i) == NA_STRING) {
      cv[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    std::string str = as<std::string>(string(i));
    bool rval = RE2::Replace(&str, re2proxy[re_idx].get(), rewrite);
    cv[i] = str;
    if (logical) {
      lv[i] = rval;
    }
  }

  return logical ? lv : cv;
}
