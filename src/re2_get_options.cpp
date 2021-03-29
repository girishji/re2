// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE

#include <Rcpp.h>
#include <re2/re2.h>

using namespace Rcpp;

// [[Rcpp::export]]
List re2_get_options(SEXP re2ptr) {
  if (TYPEOF(re2ptr) != EXTPTRSXP) {
    const char* fmt
      = "Expecting an external ptr (to RE2 object): [type=%s].";
    throw ::Rcpp::not_compatible(fmt, Rf_type2char(TYPEOF(re2ptr)));
    return List::create(0);
  }

  XPtr<RE2> xptr(re2ptr);
  auto re2p = xptr.checked_get();
  const RE2::Options& options = re2p->options();

  CharacterVector optname = CharacterVector::create(
     "encoding",
     "posix_syntax",
     "longest_match",
     "log_errors",
     "max_mem",
     "literal",
     "never_nl",
     "dot_nl",
     "never_capture",
     "case_sensitive",
     "perl_classes",
     "word_boundary",
     "one_line");
  List out(optname.size());
  out[0] = options.encoding() == RE2::Options::EncodingUTF8 ?
    "EncodingUTF8" : "EncodingLatin1";
  out[1] = options.posix_syntax();
  out[2] = options.longest_match();
  out[3] = options.log_errors();
  out[4] = options.max_mem();
  out[5] = options.literal();
  out[6] = options.never_nl();
  out[7] = options.dot_nl();
  out[8] = options.never_capture();
  out[9] = options.case_sensitive();
  out[10] = options.perl_classes();
  out[11] = options.word_boundary();
  out[12] = options.one_line();

  out.attr("names") = optname;
  return out;
}
