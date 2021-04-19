// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

static void modify_options(RE2::Options& opt,
			   Nullable<List> more_options);


//' Compile regular expression pattern
//'
//' \code{re2_regexp} compiles a character string containing a regular
//'   expression and returns a pointer to the internal representation.
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
//' @param \dots The options are (defaults in parentheses):
//'
//' \tabular{lll}{
//'   \verb{encoding} \tab (\verb{"UTF8"}) String and pattern are UTF-8;
//'                                 Otherwise \verb{"Latin1"}.\cr
//'   \verb{posix_syntax} \tab (\verb{FALSE}) Restrict regexps to POSIX egrep syntax.\cr 
//'   \verb{longest_match} \tab (\verb{FALSE}) Search for longest match, not first match.\cr
//'   \verb{max_mem} \tab (see below) Approx. max memory footprint of RE2 C++ object.\cr
//'   \verb{literal} \tab (\verb{FALSE}) Interpret pattern as literal, not regexp.\cr
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
//'
//' The \verb{max_mem} option controls how much memory can be used to
//' hold the compiled form of the regexp and its cached DFA
//' graphs (DFA: The execution engine that implements Deterministic
//' Finite Automaton search). Default is 8MB.
//'
//' @return Compiled regular expression.
//'
//' @examples
//' re2p <- re2_regexp("hello world")
//' stopifnot(mode(re2p) == "externalptr")
//'
//' ## UTF-8 and matching interface
//' # By default, pattern and input text are interpreted as UTF-8.
//' # The Latin1 option causes them to be interpreted as Latin-1.
//' x <- "fa\xE7ile"
//' Encoding(x) <- "latin1"
//' re2_detect(x, "fa\xE7")
//' stopifnot(!re2_detect(x, "fa\xE7"))
//' re <- re2_regexp("fa\xE7", encoding="Latin1")
//' re2_detect(x, re)
//' stopifnot(re2_detect(x, re))
//'
//' ## Case insensitive
//' re2_detect("fOobar ", re2_regexp("Foo", case_sensitive=FALSE))
//' res <- re2_detect("fOobar ", re2_regexp("Foo", case_sensitive=FALSE))
//' stopifnot(res == TRUE)
//' 
//' ## Use of never_nl
//' re <- re2_regexp("(abc(.|\n)*def)", never_nl=FALSE)
//' re2_match("abc\ndef\n", re)
//' res <- re2_match("abc\ndef\n", re)
//' stopifnot(res[1, 2] == "abc\ndef")
//' #
//' re <- re2_regexp("(abc(.|\n)*def)", never_nl=TRUE)
//' re2_match("abc\ndef\n", re)
//' res <- re2_match("abc\ndef\n", re)
//' stopifnot(is.na(res[1, 2]))
//'
//' @usage re2_regexp(pattern, \dots)
//'
//' @seealso \link{re2_syntax} has RE2 syntax.
//' 
// [[Rcpp::export]]
XPtr<RE2> re2_regexp(std::string& pattern,
		     Nullable<List> more_options = R_NilValue) {

  RE2::Options opt;
  modify_options(opt, more_options);

  auto re2ptr = new RE2(pattern, opt);
  if (!(re2ptr->ok())) {
    throw std::invalid_argument(re2ptr->error());
  }
  return XPtr<RE2>(re2ptr);
}

static void modify_options(RE2::Options& opt,
			   Nullable<List> more_options) {
  
  opt.set_log_errors(false); // make 'quiet' option the default

  static auto encoding_enum = [] (std::string const& val) {
    return (val == "EncodingLatin1" || val == "Latin1")
      ? RE2::Options::EncodingLatin1 : RE2::Options::EncodingUTF8;
  };
  
#define SETTER(name) else if (strcmp(R_CHAR(names(i)), #name) == 0) {   \
      opt.set_##name(as<bool>(mopts(i))); }
    
  if (more_options.isNotNull()) {
    List mopts(more_options);
    if (mopts.size() > 0) {
        StringVector names = mopts.names();

      for (int i = 0; i < names.size(); i++) {
	if (strcmp(R_CHAR(names(i)), "encoding") == 0) {
	  opt.set_encoding(encoding_enum(as<std::string>(mopts(i))));
	}
        
        SETTER(posix_syntax)
        SETTER(longest_match)
        SETTER(log_errors)
        SETTER(literal)
        SETTER(never_nl)
        SETTER(dot_nl)
        SETTER(never_capture)
        SETTER(case_sensitive)
        SETTER(perl_classes)
        SETTER(word_boundary)
        SETTER(one_line)

        else if (strcmp(R_CHAR(names(i)), "max_mem") == 0) {
          opt.set_max_mem(as<int>(mopts(i)));
        } else {
	  const char* fmt
	    = "Expecting valid option: [type=%s].";
	  throw ::Rcpp::not_compatible(fmt, R_CHAR(names(i)));
	}
      }
    }
  }
}
