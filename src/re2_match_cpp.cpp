// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include "re2_re2proxy.h"
#include <Rcpp.h>
#include <re2/re2.h>
#include <stdint.h>
#include <memory>

using namespace Rcpp;

// Extract matched groups from a string
//
// @description
// Vectorized over string. Match against a string using a
//   regular expression and extract matched substrings. Direct
//   interface to C++ function with options. See re2_match() and
//   re2_match_all() where options are already set for common
//   use.
//
// Matching regexp "(foo)|(bar)baz" on "barbazbla" will return
//   submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
//   the entire matching text. '.1' is the first group,
//   and so on. Groups can also be named.
//
// @param string A character vector, or an object which can be coerced to one.
// @param pattern Character string containing a regular expression,
//    or a pre-compiled regular expression. \cr
//   See \code{\link{re2_regexp}} for available options. \cr
//   See \link{re2_syntax} for regular expression syntax.
//
// @param \dots The options are (defaults in parentheses):
//
// \tabular{lll}{
//   \verb{startpos} \tab (\verb{0}) String offset to start matching.\cr
//   \verb{endpos} \tab (length of \verb{string}) String offset to stop
//   matching.\cr \verb{nsubmatch} \tab ('number of capturing groups' plus 1)
//                           Number of submatch entries returned.
//                           Don't ask for more match information than you
//                           will use: runs much faster with nsubmatch == 1
//                           than nsubmatch > 1, and runs even faster if
//                           nsubmatch == 0. Doesn't make sense to use
//                           nsubmatch > 1 +
//                           \code{re2_number_of_capturing_groups}, but will be
//                           handled correctly.\cr
//   \verb{re_anchor} \tab (\verb{"UNANCHORED"})
//                          "UNANCHORED" - No anchoring
//                          "ANCHOR_START" - Anchor at start only
//                          "ANCHOR_BOTH" - Anchor at start and end. \cr
//   \verb{logical} \tab (\verb{FALSE}) If TRUE, returns a logical
//                         result of TRUE if match is found.
//                         FALSE if not. Here, nsubmatch = 0.\cr
//   \verb{verbose} \tab (\verb{FALSE}) If TRUE, character
//                         matrix with map of group names and matching
//                         substrings is returned along with logical result.\cr
// }
//
// @return A character matrix mapping group names to matching
//   substrings. Column '.0' is the entire matching text. A logical
//   TRUE/FALSE vector can also be returned.
//
// @usage
//   re2_match_cpp(string, pattern, ...)
//   re2_match_cpp(string, re2_regexp(pattern, ...), ...)
//
// @examples
//
// ## Matching with substring extraction
// s <- c("barbazbla", "foobar", "this is a test")
// pat <- "(foo)|(?P<TestGroup>bar)baz"
// re2_match_cpp(s, pat)
// #
// res <- re2_match_cpp(s, pat)
// stopifnot(is.matrix(res))
// stopifnot(colnames(res) == c(".0", ".1", "TestGroup"))
// stopifnot(nrow(res) == 3, ncol(res) == 3)
// stopifnot(res[2, 1:2] == c("foo", "foo"))
//
// ## Logical result
// re2_match_cpp(s, pat, logical=TRUE)
//
// ## Compile regexp
// re <- re2_regexp("(foo)|(BAR)baz", case_sensitive=FALSE)
// re2_match_cpp(s, re)
// stopifnot(re2_match_cpp(s, re, logical=TRUE) == c(TRUE, TRUE, FALSE))
//
// ## Full anchored match
// # Successful full match
// re2_match_cpp("hello", "h.*o", re_anchor="ANCHOR_BOTH")
// stopifnot(re2_match_cpp("hello", "h.*o",
//           re_anchor="ANCHOR_BOTH", logical=TRUE))
// # Unuccessful full match
// re2_match_cpp("hello", "e", re_anchor="ANCHOR_BOTH")
// stopifnot(!re2_match_cpp("hello", "e",
//           re_anchor="ANCHOR_BOTH", logical=TRUE))
//
// ## Use of nsubmatch (ask for less)
// re2_match_cpp("ruby:1234", "(\\w+):(\\d+)", nsubmatch=1)
// stopifnot(length(re2_match_cpp("ruby:1234", "(\\w+):(\\d+)",
//                  nsubmatch=1)) == 1)
//
//
// @seealso
//   \code{\link{re2_regexp}} for options to regular expression,
//   and \link{re2_syntax} for regular expression syntax.
//
// [[Rcpp::export(.re2_match_cpp)]]
SEXP re2_match_cpp(StringVector text, SEXP pattern,
                   Nullable<List> more_options = R_NilValue) {
  bool logical = false;
  bool verbose = false;
  size_t startpos = 0;
  size_t endpos = SIZE_MAX; // a invalid value
  int nsubmatch = -1;
  RE2::Anchor anchor = RE2::UNANCHORED;

  static auto anchor_enum = [](const std::string &val) {
    std::map<std::string, RE2::Anchor> map = {
        {"UNANCHORED", RE2::UNANCHORED},
        {"ANCHOR_START", RE2::ANCHOR_START},
        {"ANCHOR_BOTH", RE2::ANCHOR_BOTH}};
    return map[val];
  };

#define NAME_IS_EQUAL(name) (strcmp(R_CHAR(names(i)), #name) == 0)

  if (more_options.isNotNull()) {
    List mopts(more_options);
    if (mopts.size() > 0) {
      StringVector names = mopts.names();
      for (int i = 0; i < names.size(); i++) {
        if (NAME_IS_EQUAL(logical) || NAME_IS_EQUAL(l)) {
          logical = as<bool>(mopts(i));
        } else if (NAME_IS_EQUAL(verbose) || NAME_IS_EQUAL(v)) {
          verbose = as<bool>(mopts(i));
        } else if (NAME_IS_EQUAL(startpos)) {
          startpos = as<size_t>(mopts(i));
        } else if (NAME_IS_EQUAL(endpos)) {
          endpos = as<size_t>(mopts(i));
        } else if (NAME_IS_EQUAL(nsubmatch)) {
          nsubmatch = as<int>(mopts(i));
        } else if (NAME_IS_EQUAL(re_anchor)) {
          StringVector sv(mopts(i));
          anchor = anchor_enum(as<std::string>(sv(0)));
        } else {
          const char *fmt = "Expecting valid option type: [type=%s].";
          throw ::Rcpp::not_compatible(fmt, R_CHAR(names(i)));
        }
      }
    }
  }

  re2::RE2Proxy re2proxy(pattern);

  nsubmatch = (nsubmatch < 0) ? re2proxy[0].nsubmatch()
                              : std::min(re2proxy[0].nsubmatch(), nsubmatch);

  LogicalVector lv(text.size());
  StringMatrix res(text.size(), nsubmatch);

  if (nsubmatch != re2proxy[0].nsubmatch()) {
    std::vector<std::string> &gn = re2proxy[0].group_names();
    std::vector<std::string> names(gn.begin(), gn.begin() + nsubmatch);
    colnames(res) = wrap(names);
  } else {
    colnames(res) = wrap(re2proxy[0].group_names());
  }

  // Maybe stack allocation is ok here. Just in case we have
  //  a lot of groups -- use heap allocation.
  std::unique_ptr<re2::StringPiece[]> submatch =
      std::unique_ptr<re2::StringPiece[]>(new re2::StringPiece[nsubmatch]);
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
    endpos = std::min(endpos, strp.size());

    if (nsubmatch == 0) {
      lv[i] =
          re2proxy[0].get().Match(strp, startpos, endpos, anchor, nullptr, 0);
    } else {
      lv[i] = re2proxy[0].get().Match(strp, startpos, endpos, anchor,
                                      submatch.get(), nsubmatch);

      for (int j = 0; j < nsubmatch; j++) {
        res(i, j) = submatch[j].data() == NULL
                        ? NA_STRING
                        : String(submatch[j].as_string());
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
    return List::create(Named("success") = lv, Named("result") = res);
  }
  return logical ? lv : res;
}
