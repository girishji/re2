// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Global replace
//'
//' @description
//' Replace successive non-overlapping occurrences of "pattern" in
//'   "text" with "rewrite" string.
//' \preformatted{
//'   re2_global_replace("yabba dabba doo", "b+", "d")
//' }
//' will result in "yada dada doo".
//' Replacements are not subject to re-matching.
//'
//' Because \verb{re2_global_replace} only replaces non-overlapping matches,
//'   replacing "ana" within "banana" makes only one replacement, not
//'   two.
//'
//' @inheritParams re2_replace
//'
//' @param \dots The options are (defaults in parentheses):
//' \tabular{lll}{
//'   \verb{count} \tab (\verb{FALSE}) If TRUE, returns the 
//'                       number of replacements made. \cr
//'   \verb{verbose} \tab (\verb{FALSE}) If TRUE, character string
//'                         or vector with replacements is returned
//'                         along with number of replacements made.\cr
//'   \verb{c} \tab (\verb{FALSE}) Same as \verb{logical} above.\cr
//'   \verb{v} \tab (\verb{FALSE}) Same as \verb{verbose} above.\cr
//' }
//' In addition, options under "RE2 Options" section below are also
//'   applicable when \verb{pattern} is given as a character string.
//'   If regexp pattern is precompiled, then options given to
//'   \code{\link{re2_re2}} take precedence.
//'
//' @inheritSection re2_re2 RE2 Options
//'
//' @return A character string or vector with replacements. An integer
//'   count of number of replacements made may also be returned
//'   depending on the options.
//'
//' @usage re2_global_replace(text, pattern, rewrite, ...)
//'
//' @examples
//' s <-  c("yabba dabba doo", "famab")
//' res <- re2_global_replace(s, "b+", "d")
//' expected <- c("yada dada doo", "famad")
//' stopifnot(res == expected)
//' #
//' re <- "(qu|[b-df-hj-np-tv-z]*)([a-z]+)"
//' rewrite <- "\\2\\1ay"
//' s <- "the quick brown fox jumps over the lazy dogs."
//' expected <- "ethay ickquay ownbray oxfay umpsjay overay ethay azylay ogsday."
//' res <- re2_global_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "\\w+"
//' rewrite <- "\\0-NOSPAM"
//' s <- "abcd.efghi@google.com"
//' expected <- "abcd-NOSPAM.efghi-NOSPAM@google-NOSPAM.com-NOSPAM"
//' res <- re2_global_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #  
//' re <- "a.*a"
//' rewrite <- "(\\0)"
//' s <- "aba\naba"
//' expected <- "(aba)\n(aba)"
//' res <- re2_global_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "b"
//' rewrite <- "bb"
//' s <- "ababababab"
//' expected <- "abbabbabbabbabb"
//' res <- re2_global_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "b+"
//' rewrite <- "bb"
//' s <- "bbbbbb"
//' expected <- "bb"
//' res <- re2_global_replace(s, re, rewrite)
//' stopifnot(res == expected)
//' #
//' re <- "b*"
//' rewrite <- "bb"
//' s <- c("bbbbbb", "aaaaa")
//' expected <- c("bb", "bbabbabbabbabbabb") 
//' res <- re2_global_replace(s, re, rewrite)
//' stopifnot(res == expected)
//'
//' @inheritSection re2_re2 Regexp Syntax
//'
//' @seealso \code{\link{re2_re2}}, \link{re2_regexp},
//'   \code{\link{re2_replace}},
//'   \code{\link{re2_match}}, \code{\link{re2_extract}}.
// [[Rcpp::export]]
SEXP re2_global_replace(StringVector text, SEXP pattern,
			std::string& rewrite,
			Nullable<List> more_options = R_NilValue) {
  
  re2::RE2Proxy re2proxy(pattern, more_options);
  StringVector sv(text.size());
  IntegerVector cntv(text.size());

  bool count = re2::RE2Proxy::is_count_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
    
  for (int i = 0; i < text.size(); i++) {
    if (text(i) == NA_STRING) {
      sv[i] = NA_STRING;
      cntv[i] = NA_INTEGER;
      continue;
    }

    std::string str = as<std::string>(text(i));
    int cnt = RE2::GlobalReplace(&str, re2proxy.get(), rewrite);
    sv[i] = str;
    if (count || verbose) {
      cntv[i] = cnt;
    }
  }

  if (verbose) {
    return List::create(Named("count") = cntv, Named("result") = sv);
  }
  return count ? cntv : sv;
}
