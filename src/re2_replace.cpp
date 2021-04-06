// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Replace the first match
//'
//' @description
//' Replace the first match of "pattern" in "text" with
//'   "rewrite" string. 
//' \preformatted{
//'   re2_replace("yabba dabba doo", "b+", "d")
//' }
//' will result in "yada dabba doo".
//'
//' @param text A character string or a character vector where
//'   replacements are sought.
//'
//' @inheritParams re2_match
//'
//' @param rewrite Rewrite string. Backslash-escaped
//'   digits (\\1 to \\9) can be used to insert text matching
//'   corresponding parenthesized group from the pattern. \\0
//'   refers to the entire matching text.
//'
//' @param \dots The options are (defaults in parentheses):
//'
//' \tabular{lll}{
//'   \verb{logical} \tab (\verb{FALSE}) If TRUE, returns a logical
//'                         result of TRUE if the pattern matches
//'                         and a replacement occurs. FALSE otherwise.\cr
//'   \verb{verbose} \tab (\verb{FALSE}) If TRUE, character string
//'                         or vector with replacements is returned
//'                         along with logical result.\cr
//'   \verb{l} \tab (\verb{FALSE}) Same as \verb{logical} above.\cr
//'   \verb{v} \tab (\verb{FALSE}) Same as \verb{verbose} above.\cr
//' }
//' In addition, options under "RE2 Options" section below are also
//'   applicable when \verb{pattern} is given as a character string.
//'   If regexp pattern is precompiled, then options given to
//'   \code{\link{re2_re2}} take precedence.
//'
//' @inheritSection re2_re2 RE2 Options
//'
//' @return A character string or vector with replacements. A logical
//'   TRUE/FALSE vector may also be returned depending on the
//'   options.
//'
//' @usage re2_replace(text, pattern, rewrite, ...)
//'
//' @inheritSection re2_re2 Regexp Syntax
//'
//' @examples
//' s <-  c("yabba dabba doo", "famab")
//' res <- re2_replace(s, "b+", "d")
//' expected <- c("yada dabba doo", "famad")
//' stopifnot(res == expected)
//' #
//' res <- re2_replace("boris@kremvax.ru",
//'                    "(.*)@([^.]*)", "\\2!\\1")
//' expected <- "kremvax!boris.ru"
//' stopifnot(res == expected)
//' #
//' re <- "(qu|[b-df-hj-np-tv-z]*)([a-z]+)"
//' rewrite <- "\\2\\1ay"
//' s <- "the quick brown fox jumps over the lazy dogs."
//' expected <- "ethay quick brown fox jumps over the lazy dogs."
//' res <- re2_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "\\w+"
//' rewrite <- "\\0-NOSPAM"
//' s <- "abcd.efghi@google.com"
//' expected <- "abcd-NOSPAM.efghi@google.com"
//' res <- re2_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #  
//' re <- "a.*a"
//' rewrite <- "(\\0)"
//' s <- "aba\naba"
//' expected <- "(aba)\naba"
//' res <- re2_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "b"
//' rewrite <- "bb"
//' s <- "ababababab"
//' expected <- "abbabababab"
//' res <- re2_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "b+"
//' rewrite <- "bb"
//' s <- "bbbbbb"
//' expected <- "bb"
//' res <- re2_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "b*"
//' rewrite <- "bb"
//' s <- c("bbbbbb", "aaaaa")
//' expected <- c("bb", "bbaaaaa")
//' res <- re2_replace(s, re, rewrite)
//' stopifnot(res == expected)
//'      
//' @seealso \code{\link{re2_re2}}, \link{re2_regexp},
//'   \code{\link{re2_global_replace}},
//'   \code{\link{re2_match}}, \code{\link{re2_extract}}.
// [[Rcpp::export]]
SEXP re2_replace(StringVector text, SEXP pattern,
		 std::string& rewrite,
		 Nullable<List> more_options = R_NilValue) {
  
  re2::RE2Proxy re2proxy(pattern, more_options);
  CharacterVector cv(text.size());
  LogicalVector lv(text.size());
  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);

  for (int i = 0; i < text.size(); i++) {
    if (text(i) == NA_STRING) {
      cv[i] = NA_STRING;
      lv[i] = NA_LOGICAL;
      continue;
    }

    std::string str = as<std::string>(text(i));
    bool rval = RE2::Replace(&str, re2proxy.get(), rewrite);
    cv[i] = str;
    if (verbose || logical) {
      lv[i] = rval;
    }
  }

  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = cv);
  }
  return logical ? lv : cv;
}
