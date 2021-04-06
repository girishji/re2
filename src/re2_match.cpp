// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Matching and substring extraction
//'
//' @description
//' Match against text using a regular expression and extract matched
//'   substrings.
//'
//' I.e., matching regexp "(foo)|(bar)baz" on "barbazbla" will return
//'   submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
//'   the entire matching text. '.1' is the first group,
//'   and so on. Groups can also be named.
//'
//' @param text A character string or a character vector where
//'   match is sought.
//' @param pattern Character string containing a regular expression,
//'    or a precompiled regular expression (see \code{\link{re2_re2}}).
//' @param \dots The options are (defaults in parentheses):
//'
//' \tabular{lll}{
//'   \verb{logical} \tab (\verb{FALSE}) If TRUE, returns a logical
//'                         result of TRUE if match is found.
//'                         FALSE if not. Here, nsubmatch = 0.\cr
//'   \verb{l} \tab (\verb{FALSE}) Same as \verb{logical} above.\cr
//'   \verb{startpos} \tab (\verb{0}) Text offset to start matching.\cr
//'   \verb{endpos} \tab (length of \verb{text}) Text offset to stop matching.\cr
//'   \verb{nsubmatch} \tab ('number of capturing groups' plus 1)
//'                           Number of submatch entries returned.
//'                           Don't ask for more match information than you
//'                           will use: runs much faster with nsubmatch == 1
//'                           than nsubmatch > 1, and runs even faster if
//'                           nsubmatch == 0. Doesn't make sense to use
//'                           nsubmatch > 1 + \code{\link{re2_number_of_capturing_groups}},
//'                           but will be handled correctly.\cr
//'   \verb{re_anchor} \tab (\verb{"UNANCHORED"})
//'                          "UNANCHORED" - No anchoring
//'                          "ANCHOR_START" - Anchor at start only
//'                          "ANCHOR_BOTH" - Anchor at start and end. \cr
//'   \verb{verbose} \tab (\verb{FALSE}) If TRUE, character
//'                         matrix with map of group names and matching
//'                         text is returned along with logical result.\cr
//'   \verb{v} \tab (\verb{FALSE}) Same as \verb{verbose} above.\cr
//' }
//' In addition, options under "RE2 Options" section below are also
//'   applicable when \verb{pattern} is given as a character string.
//'   If regexp pattern is precompiled, then options given to
//'   \code{\link{re2_re2}} take precedence.
//'
//' @inheritSection re2_re2 RE2 Options
//'
//' @return A character matrix with map of group names and matching
//'   text. Column '.0' is the entire matching text. A logical
//'   TRUE/FALSE vector may also be returned depending on the
//'   options. TRUE if match is found. FALSE if not.
//'
//' @usage re2_match(text, pattern, ...)
//'
//' @inheritSection re2_re2 Regexp Syntax
//'
//' @examples
//' #
//' ## Matching with substring extraction:
//' #
//' s <- c("barbazbla", "foobar", "this is a test")
//' pat <- "(foo)|(?P<TestGroup>bar)baz"
//' re2_match(s, pat)
//' # > re2_match(s, pat)
//' #      .0       .1    TestGroup
//' # [1,] "barbaz" NA    "bar"
//' # [2,] "foo"    "foo" NA
//' # [3,] NA       NA    NA
//' #
//' re2_match(s, pat, l=TRUE)
//' # > re2_match(s, pat, l=TRUE)
//' # [1]  TRUE  TRUE FALSE
//' #
//' stopifnot(is.matrix(re2_match(s, pat)))
//' r <- re2_match(s, pat)
//' stopifnot(colnames(r) == c(".0", ".1", "TestGroup"))
//' stopifnot(nrow(r) == 3, ncol(r) == 3)
//' stopifnot(r[2, 1:2] == c("foo", "foo"))
//' #
//' ## Compile regexp and use logical result
//' #
//' re <- re2_re2("(foo)|(?P<TestGroup>bar)baz")
//' stopifnot(re2_match(s, re, l=TRUE) == c(TRUE, TRUE, FALSE))
//' #
//' ## Full anchored match:
//' #
//' # Successful full match
//' stopifnot(re2_match("hello", "h.*o",
//'           re_anchor="ANCHOR_BOTH", l=TRUE))
//' # Unuccessful full match 
//' stopifnot(!re2_match("hello", "e",
//'           re_anchor="ANCHOR_BOTH", l=TRUE))
//' #
//' ## UTF-8 and matching interface:
//' #
//' # By default, the pattern and input text are interpreted as UTF-8.
//' # The Latin1 option causes them to be interpreted as Latin-1.
//' x <- "fa\xE7ile"
//' Encoding(x) <- "latin1"
//' x
//' stopifnot(!re2_match(x, "fa\xE7", l=TRUE))
//' stopifnot(re2_match(x, "fa\xE7", l=TRUE, encoding="Latin1"))
//' #
//' ## Use of nsubgroup (ask for less):
//' #
//' re2_match("ruby:1234", "(\\w+):(\\d+)")
//' # > re2_match("ruby:1234", "(\\w+):(\\d+)")
//' #      .0          .1     .2
//' # [1,] "ruby:1234" "ruby" "1234"
//' stopifnot(length(re2_match("ruby:1234", "(\\w+):(\\d+)")) == 3) 
//' #
//' re2_match("ruby:1234", "(\\w+):(\\d+)", nsubmatch=1)
//' # > re2_match("ruby:1234", "(\\w+):(\\d+)", nsubmatch=1)
//' #      .0
//' # [1,] "ruby:1234"
//' stopifnot(length(re2_match("ruby:1234", "(\\w+):(\\d+)",
//'                  nsubmatch=1)) == 1)
//'      
//' @seealso \code{\link{re2_re2}}, \code{\link{re2_global_replace}},
//'   \code{\link{re2_replace}}, \code{\link{re2_extract}}.
//'
// [[Rcpp::export]]
SEXP re2_match(StringVector text, SEXP pattern,
	       Nullable<List> more_options = R_NilValue) {
  
  re2::RE2Proxy re2proxy(pattern, more_options);
  LogicalVector lv(text.size());

  bool logical = re2::RE2Proxy::is_logical_out(more_options);
  bool verbose = re2::RE2Proxy::is_verbose_out(more_options);
  
  RE2::Anchor anchor = RE2::UNANCHORED;
  re2::RE2Proxy::set_option_anchor(anchor, "re_anchor", more_options);
  
  int nsubmatch = re2proxy.get().NumberOfCapturingGroups() + 1;
  re2::RE2Proxy::set_option_int(nsubmatch, "nsubmatch", more_options);
  
  std::vector<std::string> group_names(nsubmatch);
  if (nsubmatch > 0) {
    group_names[0] = ".0";
    const std::map<int, std::string>& cgroups
      = re2proxy.get().CapturingGroupNames();
    for (int i = 1; i < nsubmatch; i++) {
      auto search = cgroups.find(i);
      group_names[i] = search != cgroups.end()
	? search->second : "." + std::to_string(i);
    }
  }

  StringMatrix res(text.size(), nsubmatch);
  if (nsubmatch > 0) {
    colnames(res) = wrap(group_names);
  }
  size_t startpos = 0;
  re2::RE2Proxy::set_option_uint(startpos, "startpos", more_options);

  // Maybe stack allocation is ok here. Just in case we have
  //  a lot of groups -- use heap allocation.
  std::unique_ptr<re2::StringPiece[]> submatch
    = std::unique_ptr<re2::StringPiece[]>(new re2::StringPiece[nsubmatch]);
  // re2::StringPiece submatch[nsubmatch];

  for (int i = 0; i < text.size(); i++) {
    
    if (text(i) == NA_STRING) {
      for (int j = 0; j < nsubmatch; j++) {
        res(i, j) = NA_STRING;
      }
      lv[i] = NA_LOGICAL;
      continue;
    }

    re2::StringPiece strp(R_CHAR(text(i))); // shallow copy (providing char *)
    size_t endpos = strp.size();
    re2::RE2Proxy::set_option_uint(endpos, "endpos", more_options);
    
    if (nsubmatch == 0) {
      lv[i] = re2proxy.get().Match(strp, startpos, endpos, anchor,
				   nullptr, 0);
    } else {
      lv[i] = re2proxy.get().Match(strp, startpos, endpos, anchor,
				   submatch.get(), nsubmatch);
				   
      for (int j = 0; j < nsubmatch; j++) {
        res(i, j) = submatch[j].data() == NULL
	  ? NA_STRING : String(submatch[j].as_string());
      }
    }
    // clear
    for (int sm = 0; sm < nsubmatch; sm++) {
      submatch[sm] = re2::StringPiece(); 
    }
  } // for
  
  if (nsubmatch == 0) {
    return lv;
  }
  if (verbose) {
    return List::create(Named("success") = lv,
			Named("result") = res);
  }
  return logical ? lv : res;
}
