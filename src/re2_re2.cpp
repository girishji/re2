// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

//' Compile regex pattern
//'
//' \code{re2_re2} compiles a character string containing a regular
//'   expression and returns a pointer to the internal representation.
//'   The returned value can be used (instead of
//'   character string pattern) in \code{\link{re2_match}},
//'   \code{\link{re2_replace}}, \code{\link{re2_global_replace}},
//'   \code{\link{re2_extract}}, etc. for efficiency.
//'
//' @section Regexp Syntax:
//'
//' RE2 regular expression syntax is similar to Perl's with some of
//'   the more complicated things thrown away. In particular,
//'   backreferences and generalized assertions are not available, nor
//'   is \verb{\Z}.
//'
//' See \link{re2_syntax} for the syntax
//'   supported by RE2, and a comparison with PCRE and PERL regexps.
//'
//' For those not familiar with Perl's regular expressions, here are
//'   some examples of the most commonly used extensions:
//' \tabular{lll}{
//'   \verb{"hello (\\w+) world"} \tab -- \tab \\w matches a "word" character. \cr
//'   \verb{"version (\\d+)"}     \tab -- \tab \\d matches a digit. \cr
//'   \verb{"hello\\s+world"}     \tab -- \tab \\s matches any whitespace character. \cr
//'   \verb{"\\b(\\w+)\\b"}       \tab -- \tab \\b matches non-empty string at word boundary. \cr
//'   \verb{"(?i)hello"}        \tab -- \tab (?i) turns on case-insensitive matching. \cr
//'   \verb{"/\\*(.*?)\\*/"}      \tab -- \tab \verb{.*?} matches . minimum no. of times possible.
//' }
//' The double backslashes are needed when writing R string literals.
//' However, they should NOT be used when writing raw string literals:
//' \tabular{lll}{
//'   \verb{r"(hello (\w+) world)"} \tab -- \tab \\w matches a "word" character. \cr
//'   \verb{r"(version (\d+))"}     \tab -- \tab \\d matches a digit. \cr
//'   \verb{r"(hello\s+world)"}     \tab -- \tab \\s matches any whitespace character. \cr
//'   \verb{r"(\b(\w+)\b)"}       \tab -- \tab \\b matches non-empty string at word boundary. \cr
//'   \verb{r"((?i)hello)"}        \tab -- \tab (?i) turns on case-insensitive matching. \cr
//'   \verb{r"(/\*(.*?)\*/)"}      \tab -- \tab \verb{.*?} matches . minimum no. of times possible.
//' }
//' When using UTF-8 encoding, case-insensitive matching will perform
//' simple case folding, not full case folding.
//'
//' @param pattern Character string containing a
//' regular expression.
//'
//' @param \dots Options, as comma separated \verb{option=value}. See "RE2 Options" section below.
//'
//' @section RE2 Options:
//' The options are (defaults in parentheses):
//'
//' \tabular{lll}{
//'   \verb{encoding} \tab (\verb{"UTF8"}) Text and pattern are UTF-8;
//'                                 Otherwise \verb{"Latin1"}.\cr
//'   \verb{posix_syntax} \tab (\verb{FALSE}) Restrict regexps to POSIX egrep syntax.\cr 
//'   \verb{longest_match} \tab (\verb{FALSE}) Search for longest match, not first match.\cr
//'   \verb{max_mem} \tab (see below) Approx. max memory footprint of RE2.\cr
//'   \verb{literal} \tab (\verb{FALSE}) Interpret string as literal, not regexp.\cr
//'   \verb{never_nl} \tab (\verb{FALSE}) Never match \\n, even if it is in regexp.\cr
//'   \verb{dot_nl} \tab (\verb{FALSE}) Dot matches everything including new line.\cr
//'   \verb{never_capture} \tab (\verb{FALSE}) Parse all parens as non-capturing.\cr
//'   \verb{case_sensitive} \tab (\verb{TRUE}) Match is case-sensitive (regexp can 
//'                                      override with (?i) unless in posix_syntax mode).\cr
//' }
//' The following options are only consulted when \verb{posix_syntax=TRUE}.
//' When \verb{posix_syntax=FALSE}, these features are always enabled and
//' cannot be turned off; to perform multi-line matching in that case,
//' begin the regexp with (?m). 
//' \tabular{lll}{
//'   \verb{perl_classes} \tab (\verb{FALSE}) Allow Perl's \verb{\\d \\s \\w \\D \\S \\W}.\cr
//'   \verb{word_boundary} \tab (\verb{FALSE}) Allow Perl's \verb{\\b \\B} (word boundary and not).\cr
//'   \verb{one_line} \tab (\verb{FALSE}) \verb{^} and \verb{$} only match beginning and end of text.\cr
//' }
//' The \verb{max_mem} option controls how much memory can be used
//' to hold the compiled form of the regexp (the Prog) and
//' its cached DFA graphs. Code Search placed limits on the number
//' of Prog instructions and DFA states: 10,000 for both.
//' In RE2, those limits would translate to about 240 KB per Prog
//' and perhaps 2.5 MB per DFA (DFA state sizes vary by regexp; RE2 does a
//' better job of keeping them small than Code Search did).
//' Each RE2 has two Progs (one forward, one reverse), and each Prog
//' can have two DFAs (one first match, one longest match). \cr
//' That makes 4 DFAs: 
//' \tabular{lll}{
//'   forward, first-match   \tab - \tab used for \verb{UNANCHORED} or \verb{ANCHOR_START} 
//'                             searches if \verb{longest_match=FALSE}. \cr
//'   forward, longest-match \tab - \tab used for all \verb{ANCHOR_BOTH} searches,
//'                             and the other two kinds if 
//'                             \verb{longest_match=TRUE}. \cr
//'   reverse, first-match   \tab - \tab never used. \cr
//'   reverse, longest-match \tab - \tab used as second phase for unanchored
//'                                      searches. \cr
//' }
//' The RE2 memory budget is statically divided between the two
//' Progs and then the DFAs: two thirds to the forward Prog
//' and one third to the reverse Prog.  The forward Prog gives half
//' of what it has left over to each of its DFAs.  The reverse Prog
//' gives it all to its longest-match DFA.
//'
//' Once a DFA fills its budget, it flushes its cache and starts over.
//' If this happens too often, RE2 falls back on the NFA implementation.
//' 
//' @return Compiled regular expression.
//'
//' @examples
//' re2p <- re2_re2("hello world")
//' stopifnot(mode(re2p) == "externalptr")
//' 
//' re2p <- re2_re2("Ruby:1234", case_sensitive=FALSE)
//' stopifnot(mode(re2p) == "externalptr")
//'
//' @usage re2_re2(pattern, \dots)
//'
//' @seealso \link{re2_syntax},  \code{\link{re2_replace}}, \code{\link{re2_global_replace}},
//'   \code{\link{re2_match}}, \code{\link{re2_extract}}.
// [[Rcpp::export]]
XPtr<RE2> re2_re2(std::string& pattern,
		  Nullable<List> more_options = R_NilValue) {

  RE2::Options opt;
  re2::RE2Proxy::modify_options(opt, more_options);

  auto re2ptr = new RE2(pattern, opt);
  if (!(re2ptr->ok())) {
    throw std::invalid_argument(re2ptr->error());
  }
  return XPtr<RE2>(re2ptr);
}
