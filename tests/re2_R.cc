// XXX copyright

// Copyright 2003-2009 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Regular expression interface RE2.
//
// Originally the PCRE C++ wrapper, but adapted to use
// the new automata-based regular expression engines.

#include "re2/re2.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <atomic>
#include <iterator>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "util/util.h"
#include "util/logging.h"
#include "util/strutil.h"
#include "util/utf.h"
#include "re2/prog.h"
#include "re2/regexp.h"
#include "re2/sparse_array.h"

#include <RInside.h>                    // for the embedded R via RInside
#include <random>

namespace re2 {

// Maximum number of args we can set
static const int kMaxArgs = 16;
static const int kVecSize = 1+kMaxArgs;

// RInside object has a destructor that calls Rf_endEmbeddedR(0) to
//   to stop embedded R. However destructor will not get called
//   if this is compiled into dynamic libarary
// https://stackoverflow.com/questions/38510621/destructor-of-a-global-static-variable-in-a-shared-library-is-not-called-on-dlcl
// In essence, this simple solution of a file hidden global variable
//   works only for static linking.
static RInside R;              // create an embedded R instance
static void embed_re2(const RE2 &re2);
  
static std::random_device rd;
static std::mt19937_64 gen(rd());

const int RE2::Options::kDefaultMaxMem;  // initialized in re2.h

RE2::Options::Options(RE2::CannedOptions opt)
  : encoding_(opt == RE2::Latin1 ? EncodingLatin1 : EncodingUTF8),
    posix_syntax_(opt == RE2::POSIX),
    longest_match_(opt == RE2::POSIX),
    log_errors_(opt != RE2::Quiet),
    max_mem_(kDefaultMaxMem),
    literal_(false),
    never_nl_(false),
    dot_nl_(false),
    never_capture_(false),
    case_sensitive_(true),
    perl_classes_(false),
    word_boundary_(false),
    one_line_(false) {
}

// static empty objects for use as const references.
// To avoid global constructors, allocated in RE2::Init().
static const std::string* empty_string;
static const std::map<std::string, int>* empty_named_groups;
static const std::map<int, std::string>* empty_group_names;

// Converts from Regexp error code to RE2 error code.
// Maybe some day they will diverge.  In any event, this
// hides the existence of Regexp from RE2 users.
static RE2::ErrorCode RegexpErrorToRE2(re2::RegexpStatusCode code) {
  switch (code) {
    case re2::kRegexpSuccess:
      return RE2::NoError;
    case re2::kRegexpInternalError:
      return RE2::ErrorInternal;
    case re2::kRegexpBadEscape:
      return RE2::ErrorBadEscape;
    case re2::kRegexpBadCharClass:
      return RE2::ErrorBadCharClass;
    case re2::kRegexpBadCharRange:
      return RE2::ErrorBadCharRange;
    case re2::kRegexpMissingBracket:
      return RE2::ErrorMissingBracket;
    case re2::kRegexpMissingParen:
      return RE2::ErrorMissingParen;
    case re2::kRegexpUnexpectedParen:
      return RE2::ErrorUnexpectedParen;
    case re2::kRegexpTrailingBackslash:
      return RE2::ErrorTrailingBackslash;
    case re2::kRegexpRepeatArgument:
      return RE2::ErrorRepeatArgument;
    case re2::kRegexpRepeatSize:
      return RE2::ErrorRepeatSize;
    case re2::kRegexpRepeatOp:
      return RE2::ErrorRepeatOp;
    case re2::kRegexpBadPerlOp:
      return RE2::ErrorBadPerlOp;
    case re2::kRegexpBadUTF8:
      return RE2::ErrorBadUTF8;
    case re2::kRegexpBadNamedCapture:
      return RE2::ErrorBadNamedCapture;
  }
  return RE2::ErrorInternal;
}

static std::string trunc(const StringPiece& pattern) {
  if (pattern.size() < 100)
    return std::string(pattern);
  return std::string(pattern.substr(0, 100)) + "...";
}


RE2::RE2(const char* pattern) {
  Init(pattern, DefaultOptions);
}

RE2::RE2(const std::string& pattern) {
  Init(pattern, DefaultOptions);
}

RE2::RE2(const StringPiece& pattern) {
  Init(pattern, DefaultOptions);
}

RE2::RE2(const StringPiece& pattern, const Options& options) {
  Init(pattern, options);
}

int RE2::Options::ParseFlags() const {
  int flags = Regexp::ClassNL;
  switch (encoding()) {
    default:
      if (log_errors())
        LOG(ERROR) << "Unknown encoding " << encoding();
      break;
    case RE2::Options::EncodingUTF8:
      break;
    case RE2::Options::EncodingLatin1:
      flags |= Regexp::Latin1;
      break;
  }

  if (!posix_syntax())
    flags |= Regexp::LikePerl;

  if (literal())
    flags |= Regexp::Literal;

  if (never_nl())
    flags |= Regexp::NeverNL;

  if (dot_nl())
    flags |= Regexp::DotNL;

  if (never_capture())
    flags |= Regexp::NeverCapture;

  if (!case_sensitive())
    flags |= Regexp::FoldCase;

  if (perl_classes())
    flags |= Regexp::PerlClasses;

  if (word_boundary())
    flags |= Regexp::PerlB;

  if (one_line())
    flags |= Regexp::OneLine;

  return flags;
}

void RE2::Init(const StringPiece& pattern, const Options& options) {
  static std::once_flag empty_once;
  std::call_once(empty_once, []() {
    empty_string = new std::string;
    empty_named_groups = new std::map<std::string, int>;
    empty_group_names = new std::map<int, std::string>;
  });

  pattern_.assign(pattern.data(), pattern.size());
  options_.Copy(options);
  entire_regexp_ = NULL;
  error_ = empty_string;
  error_code_ = NoError;
  error_arg_.clear();
  prefix_.clear();
  prefix_foldcase_ = false;
  suffix_regexp_ = NULL;
  prog_ = NULL;
  num_captures_ = -1;
  is_one_pass_ = false;

  rprog_ = NULL;
  named_groups_ = NULL;
  group_names_ = NULL;

  RegexpStatus status;
  entire_regexp_ = Regexp::Parse(
    pattern_,
    static_cast<Regexp::ParseFlags>(options_.ParseFlags()),
    &status);
  if (entire_regexp_ == NULL) {
    if (options_.log_errors()) {
      LOG(ERROR) << "Error parsing '" << trunc(pattern_) << "': "
                 << status.Text();
    }
    error_ = new std::string(status.Text());
    error_code_ = RegexpErrorToRE2(status.code());
    error_arg_ = std::string(status.error_arg());
    return;
  }

  re2::Regexp* suffix;
  if (entire_regexp_->RequiredPrefix(&prefix_, &prefix_foldcase_, &suffix))
    suffix_regexp_ = suffix;
  else
    suffix_regexp_ = entire_regexp_->Incref();

  // Two thirds of the memory goes to the forward Prog,
  // one third to the reverse prog, because the forward
  // Prog has two DFAs but the reverse prog has one.
  prog_ = suffix_regexp_->CompileToProg(options_.max_mem()*2/3);
  if (prog_ == NULL) {
    if (options_.log_errors())
      LOG(ERROR) << "Error compiling '" << trunc(pattern_) << "'";
    error_ = new std::string("pattern too large - compile failed");
    error_code_ = RE2::ErrorPatternTooLarge;
    return;
  }

  // We used to compute this lazily, but it's used during the
  // typical control flow for a match call, so we now compute
  // it eagerly, which avoids the overhead of std::once_flag.
  num_captures_ = suffix_regexp_->NumCaptures();

  // Could delay this until the first match call that
  // cares about submatch information, but the one-pass
  // machine's memory gets cut from the DFA memory budget,
  // and that is harder to do if the DFA has already
  // been built.
  is_one_pass_ = prog_->IsOnePass();
}

// Returns rprog_, computing it if needed.
re2::Prog* RE2::ReverseProg() const {
  std::call_once(rprog_once_, [](const RE2* re) {
    re->rprog_ =
        re->suffix_regexp_->CompileToReverseProg(re->options_.max_mem() / 3);
    if (re->rprog_ == NULL) {
      if (re->options_.log_errors())
        LOG(ERROR) << "Error reverse compiling '" << trunc(re->pattern_) << "'";
      // We no longer touch error_ and error_code_ because failing to compile
      // the reverse Prog is not a showstopper: falling back to NFA execution
      // is fine. More importantly, an RE2 object is supposed to be logically
      // immutable: whatever ok() would have returned after Init() completed,
      // it should continue to return that no matter what ReverseProg() does.
    }
  }, this);
  return rprog_;
}

RE2::~RE2() {
  if (suffix_regexp_)
    suffix_regexp_->Decref();
  if (entire_regexp_)
    entire_regexp_->Decref();
  delete prog_;
  delete rprog_;
  if (error_ != empty_string)
    delete error_;
  if (named_groups_ != NULL && named_groups_ != empty_named_groups)
    delete named_groups_;
  if (group_names_ != NULL &&  group_names_ != empty_group_names)
    delete group_names_;
}

int RE2::ProgramSize() const {
  if (prog_ == NULL)
    return -1;
  return prog_->size();
}

int RE2::ReverseProgramSize() const {
  if (prog_ == NULL)
    return -1;
  Prog* prog = ReverseProg();
  if (prog == NULL)
    return -1;
  return prog->size();
}

// Finds the most significant non-zero bit in n.
static int FindMSBSet(uint32_t n) {
  DCHECK_NE(n, 0);
#if defined(__GNUC__)
  return 31 ^ __builtin_clz(n);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
  unsigned long c;
  _BitScanReverse(&c, n);
  return static_cast<int>(c);
#else
  int c = 0;
  for (int shift = 1 << 4; shift != 0; shift >>= 1) {
    uint32_t word = n >> shift;
    if (word != 0) {
      n = word;
      c += shift;
    }
  }
  return c;
#endif
}

static int Fanout(Prog* prog, std::vector<int>* histogram) {
  SparseArray<int> fanout(prog->size());
  prog->Fanout(&fanout);
  int data[32] = {};
  int size = 0;
  for (SparseArray<int>::iterator i = fanout.begin(); i != fanout.end(); ++i) {
    if (i->value() == 0)
      continue;
    uint32_t value = i->value();
    int bucket = FindMSBSet(value);
    bucket += value & (value-1) ? 1 : 0;
    ++data[bucket];
    size = std::max(size, bucket+1);
  }
  if (histogram != NULL)
    histogram->assign(data, data+size);
  return size-1;
}

int RE2::ProgramFanout(std::vector<int>* histogram) const {
  if (prog_ == NULL)
    return -1;
  return Fanout(prog_, histogram);
}

int RE2::ReverseProgramFanout(std::vector<int>* histogram) const {
  if (prog_ == NULL)
    return -1;
  Prog* prog = ReverseProg();
  if (prog == NULL)
    return -1;
  return Fanout(prog, histogram);
}

// Returns named_groups_, computing it if needed.
const std::map<std::string, int>& RE2::NamedCapturingGroups() const {

  static std::map<std::string, int> result;
  result.clear();

  try {
    std::string txt = "suppressMessages(library(re2))";
    R.parseEvalQ(txt); 
	
    R["pattern"] = this->pattern();
    Rcpp::Nullable<Rcpp::IntegerVector> ngrpids
      = R.parseEval("re2_named_capturing_groups(pattern)");
    if (ngrpids.isNull()) {
      return result;
    }
    Rcpp::IntegerVector grpids(ngrpids);
    Rcpp::StringVector names = grpids.attr("names");
    for (int i = 0; i < names.size(); i++) {
      result[Rcpp::as<std::string>(names[i])] = grpids[i];
    }
    
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return result;
}

// Returns group_names_, computing it if needed.
const std::map<int, std::string>& RE2::CapturingGroupNames() const {

  static std::map<int, std::string> result;
  result.clear();

  try {
    std::string txt = "suppressMessages(library(re2))";
    R.parseEvalQ(txt); 
	
    R["pattern"] = this->pattern();
    Rcpp::Nullable<Rcpp::StringVector> grpnames
      = R.parseEval("re2_capturing_group_names(pattern)");
    if (grpnames.isNull()) {
      return result;
    }
    Rcpp::StringVector names(grpnames);
    Rcpp::StringVector grpids = names.attr("names");
    for (int i = 0; i < names.size(); i++) {
      result[std::stoi(Rcpp::as<std::string>(grpids[i]))]
	= Rcpp::as<std::string>(names[i]);
    }
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return result;
}

/***** Convenience interfaces *****/

bool RE2::FullMatchN(const StringPiece& text, const RE2& re,
                     const Arg* const args[], int n) {
  return re.DoMatch(text, ANCHOR_BOTH, NULL, args, n);
}

bool RE2::PartialMatchN(const StringPiece& text, const RE2& re,
                        const Arg* const args[], int n) {
  return re.DoMatch(text, UNANCHORED, NULL, args, n);
}

bool RE2::ConsumeN(StringPiece* input, const RE2& re,
                   const Arg* const args[], int n) {
  size_t consumed;
  if (re.DoMatch(*input, ANCHOR_START, &consumed, args, n)) {
    input->remove_prefix(consumed);
    return true;
  } else {
    return false;
  }
}

bool RE2::FindAndConsumeN(StringPiece* input, const RE2& re,
                          const Arg* const args[], int n) {
  size_t consumed;
  if (re.DoMatch(*input, UNANCHORED, &consumed, args, n)) {
    input->remove_prefix(consumed);
    return true;
  } else {
    return false;
  }
}

bool RE2::Replace(std::string* str,
                  const RE2& re,
                  const StringPiece& rewrite) {

  std::uniform_int_distribution<int> dist(0, 3); //0,1,2,3

  try {
    std::string txt = "suppressMessages(library(re2))";
    R.parseEvalQ(txt);              // load library, no return value

    R["tstr"] = *str;
    R["tpat"] = re.pattern();
    R["trewr"] = rewrite.as_string();
    
    if (dist(gen) == 0) {
      Rcpp::StringVector result = R.parseEval("re2_replace(tstr, tpat, trewr)");
      std::string changed = Rcpp::as< std::string >(result(0));      
      *str = changed;
      Rcpp::LogicalVector lv
	= R.parseEval("re2_replace(tstr, tpat, trewr, logical=T)");
      return lv(0);

    } else if (dist(gen) == 1) {

      Rcpp::List result2 = R.parseEval("re2_replace(tstr, tpat, trewr, verbose=T)");
      Rcpp::LogicalVector lv = result2["success"];
      Rcpp::StringVector sv = result2["result"];
      std::string changed = Rcpp::as< std::string >(sv(0));
      *str = changed;
      return lv(0);

    } else if (dist(gen) == 2) {

      Rcpp::StringVector result
	= R.parseEval("rptr <- re2_re2(tpat); re2_replace(tstr, rptr, trewr)");
      std::string changed = Rcpp::as< std::string >(result(0));
      *str = changed;
      Rcpp::LogicalVector lv
	= R.parseEval("rptr <- re2_re2(tpat); re2_replace(tstr, rptr, trewr, logical=T)");
      return lv(0);

    } else {
      Rcpp::List result2 = R.parseEval("rptr <- re2_re2(tpat); re2_replace(tstr, rptr, trewr, verbose=T)");
      Rcpp::LogicalVector lv = result2["success"];
      Rcpp::StringVector sv = result2["result"];
      std::string changed = Rcpp::as< std::string >(sv(0));
      *str = changed;
      return lv(0);      
    }
    
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return false;
}

int RE2::GlobalReplace(std::string* str,
                       const RE2& re,
                       const StringPiece& rewrite) {

  std::uniform_int_distribution<int> dist(0, 3); //0,1,2,3

  try {
    std::string txt = "suppressMessages(library(re2))";
    R.parseEvalQ(txt);              // load library, no return value

    R["tstr"] = *str;
    R["tpat"] = re.pattern();
    R["trewr"] = rewrite.as_string();
    
    if (dist(gen) == 0) {
      Rcpp::StringVector result = R.parseEval("re2_global_replace(tstr, tpat, trewr)");
      std::string changed = Rcpp::as< std::string >(result(0));      
      *str = changed;
      Rcpp::IntegerVector lv = R.parseEval("re2_global_replace(tstr, tpat, trewr, count=T)");
      return lv[0];

    } else if (dist(gen) == 1) {

      Rcpp::List result2 = R.parseEval("re2_global_replace(tstr, tpat, trewr, verbose=T)");
      Rcpp::IntegerVector lv = result2["count"];
      Rcpp::StringVector sv = result2["result"];
      std::string changed = Rcpp::as< std::string >(sv(0));
      *str = changed;
      return lv[0];

    } else if (dist(gen) == 2) {

      Rcpp::StringVector result = R.parseEval("rptr <- re2_re2(tpat); re2_global_replace(tstr, rptr, trewr)");
      std::string changed = Rcpp::as< std::string >(result(0));
      *str = changed;
      Rcpp::IntegerVector lv
	= R.parseEval("rptr <- re2_re2(tpat); re2_global_replace(tstr, rptr, trewr, count=T)");
      return lv[0];

    } else {
      Rcpp::List result2 = R.parseEval("rptr <- re2_re2(tpat); re2_global_replace(tstr, rptr, trewr, verbose=T)");
      Rcpp::IntegerVector lv = result2["count"];
      Rcpp::StringVector sv = result2["result"];
      std::string changed = Rcpp::as< std::string >(sv(0));
      *str = changed;
      return lv[0];      
    }
    
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return false;
}

bool RE2::Extract(const StringPiece& text,
                  const RE2& re,
                  const StringPiece& rewrite,
                  std::string* out) {
  try {
    embed_re2(re);

    R["text_"] = text.as_string();
    R["rewrite_"] = rewrite.as_string();
    std::string evalstr = "re2_extract(text_, re2ptr, rewrite_";
    Rcpp::LogicalVector lv = R.parseEval(evalstr + ", l=T)");     
    Rcpp::StringVector result = R.parseEval(evalstr + ")");
    
    if (result(0) != NA_STRING) {
      *out = result(0);
    }
    return lv(0);
      
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return false;
}


int RE2::NumberOfCapturingGroups() const {
 
  try {
    embed_re2(*this);
    	
    Rcpp::IntegerVector result
      = R.parseEval("re2_number_of_capturing_groups(re2ptr)");
    return Rcpp::as<int>(result);
    
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return -1;
}
 
 

static bool has_multiple_null(const StringPiece& unquoted) {
  // Some tests (HasNull) test for multiple null in the stirng. Bypass them,
  // since R does not allow multiple null in the middle of string.
  // Note: C++ std::string is NOT \0-terminated.
  // However, you can extract a pointer to an internal buffer that contains a
  // C-String with the method c_str().
  //  
  if (unquoted.as_string().length() != strlen(unquoted.as_string().c_str())) {
    return true;
  }
  return false;
}
  
std::string RE2::QuoteMeta(const StringPiece& unquoted) {
     
  if (has_multiple_null(unquoted)) {
    return unquoted.as_string();
  }
 
  try {
    std::string txt = "suppressMessages(library(re2))";
    //std::string txt = "library(re2)";
    R.parseEvalQ(txt);              // load library, no return value
	
    R["txt"] = unquoted.as_string();   // assign a char* (string) to 'txt'
    Rcpp::StringVector result = R.parseEval("re2_quote_meta(txt)");
    return Rcpp::as< std::string >(result[0]);
    
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return "error";
}

bool RE2::PossibleMatchRange(std::string* min, std::string* max,
                             int maxlen) const {

  try {
    embed_re2(*this);

    R["maxlen_"] = maxlen;
    std::string evalstr = "re2_possible_match_range(re2ptr, maxlen_";
    SEXP lv = R.parseEval(evalstr + ", l=T)");
    Rcpp::StringVector result = R.parseEval(evalstr + ")");

    if (result(0) != NA_STRING) {
      *min = Rcpp::as<std::string>(result(0));
    }
    if (result(1) != NA_STRING) {
      *max = Rcpp::as<std::string>(result(1));
    }
    return Rcpp::as<bool>(lv);
      
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return false;
}

static void embed_re2_inner(const std::string &pattern, const RE2::Options &options) {

  std::string txt = "suppressMessages(library(re2))";
  R.parseEvalQ(txt);              // load library, no return value

  R["pattern"] = pattern;
  std::string cmd = "re2ptr <- re2_re2(pattern";

  if (options.encoding() == RE2::Options::EncodingUTF8) {
    R["encoding_"] = "EncodingUTF8";
  } else {
    R["encoding_"] = "EncodingLatin1";        
  }
  cmd += ", encoding=encoding_";

  cmd += std::string(", posix_syntax=") +   (options.posix_syntax() ? "T" : "F");
  cmd += std::string(", longest_match=") +  (options.longest_match() ? "T" : "F");
  cmd += std::string(", log_errors=") +     (options.log_errors() ? "T" : "F");
  cmd += std::string(", max_mem=") +        std::to_string(options.max_mem());
  cmd += std::string(", literal=") +        (options.literal() ? "T" : "F");	       
  cmd += std::string(", never_nl=") +       (options.never_nl() ? "T" : "F");	       
  cmd += std::string(", dot_nl=") +	    (options.dot_nl() ? "T" : "F");	       
  cmd += std::string(", never_capture=") +  (options.never_capture() ? "T" : "F");   
  cmd += std::string(", case_sensitive=") + (options.case_sensitive() ? "T" : "F");  
  cmd += std::string(", perl_classes=") + (options.perl_classes() ? "T" : "F");    
  cmd += std::string(", word_boundary=") +(options.word_boundary() ? "T" : "F");   
  cmd += std::string(", one_line=") +     (options.one_line() ? "T" : "F");        
  cmd += ")";

  R.parseEvalQ(cmd);

  // print
  //Rcpp::List olist = R.parseEval("re2_get_options(re2ptr)");
  //std::cout << "encoding " << Rcpp::as<std::string>(olist["encoding"]) << '\n'
  //	    << "posix_syntax " << Rcpp::as<bool>(olist["posix_syntax"]) << '\n'
  //	    << "longest_match " << Rcpp::as<bool>(olist["longest_match"]) << '\n'
  //	    << "log_errors " << Rcpp::as<bool>(olist["log_errors"]) << '\n'
  //	    << "max_mem " << Rcpp::as<int>(olist["max_mem"]) << '\n'
  //	    << "literal " << Rcpp::as<bool>(olist["literal"]) << '\n'
  //	    << "never_nl " << Rcpp::as<bool>(olist["never_nl"]) << '\n'
  //	    << "dot_nl " << Rcpp::as<bool>(olist["dot_nl"]) << '\n'
  //	    << "never_capture " << Rcpp::as<bool>(olist["never_capture"]) << '\n'
  //	    << "case_sensitive " << Rcpp::as<bool>(olist["case_sensitive"]) << '\n'
  //	    << "perl_classes " << Rcpp::as<bool>(olist["perl_classes"]) << '\n'
  //	    << "word_boundary " << Rcpp::as<bool>(olist["word_boundary"]) << '\n'
  //	    << "one_line " << Rcpp::as<bool>(olist["one_line"]) << '\n';
  //
}

static void embed_re2(const RE2 &re2) {
  embed_re2_inner(re2.pattern(), re2.options());
}
  

/***** Actual matching and rewriting code *****/

bool RE2::Match(const StringPiece& text,
                size_t startpos,
                size_t endpos,
                Anchor re_anchor,
                StringPiece* submatch,
                int nsubmatch) const {

  //std::cerr << "Match: text:" << text
  //	    << " :pattern:" << this->pattern()
  //	    << ": sp:" << startpos
  //	    << " ep:" << endpos
  //	    << " anchor:" << re_anchor
  //	    << " nsubmatch: " << nsubmatch
  //	    << std::endl;

  // Note: QuoteMeta HasNull test re2_test.cc does not pass in R
  //  because of multiple embedded \0 in string.
  
  try {
    embed_re2(*this);

    R["text_"] = text.as_string();
    R["startpos_"] = startpos;
    R["endpos_"] = endpos;
    R["nsubmatch_"] = nsubmatch;
    if (re_anchor == UNANCHORED) {
      R["re_anchor_"] = "UNANCHORED";
    } else if (re_anchor == ANCHOR_START) {
      R["re_anchor_"] = "ANCHOR_START";
    } else {
      R["re_anchor_"] = "ANCHOR_BOTH";
    }
    
    std::string evalstr = "re2_match(text_, re2ptr, startpos=startpos_, "
      "endpos=endpos_, re_anchor=re_anchor_, "
      "nsubmatch=nsubmatch_, testing=T";
    Rcpp::LogicalVector lv = R.parseEval(evalstr + ", logical=T)");
    Rcpp::StringVector result = R.parseEval(evalstr + ")");
    
    for (int i = 0; i < nsubmatch; i++) {
      if (result(i) != NA_STRING) {
    	submatch[i] = StringPiece(result(i));
      } else {
    	submatch[i] = StringPiece();
      }
    }
    return lv(0);
      
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return false;

}

// Internal matcher - like Match() but takes Args not StringPieces.
bool RE2::DoMatch(const StringPiece& text,
                  Anchor re_anchor,
                  size_t* consumed,
                  const Arg* const* args,
                  int n) const {
  if (!ok()) {
    if (options_.log_errors())
      LOG(ERROR) << "Invalid RE2: " << *error_;
    return false;
  }

  if (NumberOfCapturingGroups() < n) {
    // RE has fewer capturing groups than number of Arg pointers passed in.
    return false;
  }

  // Count number of capture groups needed.
  int nvec;
  if (n == 0 && consumed == NULL)
    nvec = 0;
  else
    nvec = n+1;

  StringPiece* vec;
  StringPiece stkvec[kVecSize];
  StringPiece* heapvec = NULL;

  if (nvec <= static_cast<int>(arraysize(stkvec))) {
    vec = stkvec;
  } else {
    vec = new StringPiece[nvec];
    heapvec = vec;
  }

  if (!Match(text, 0, text.size(), re_anchor, vec, nvec)) {
    delete[] heapvec;
    return false;
  }

  if (consumed != NULL)
    *consumed = static_cast<size_t>(vec[0].end() - text.begin());

  if (n == 0 || args == NULL) {
    // We are not interested in results
    delete[] heapvec;
    return true;
  }

  // If we got here, we must have matched the whole pattern.
  for (int i = 0; i < n; i++) {
    const StringPiece& s = vec[i+1];
    if (!args[i]->Parse(s.data(), s.size())) {
      // TODO: Should we indicate what the error was?
      delete[] heapvec;
      return false;
    }
  }

  delete[] heapvec;
  return true;
}

// Checks that the rewrite string is well-formed with respect to this
// regular expression.
bool RE2::CheckRewriteString(const StringPiece& rewrite,
                             std::string* error) const {
  try {
    embed_re2(*this);

    R["rewrite_"] = rewrite.as_string();
    std::string evalstr = "re2_check_rewrite_string(re2ptr, rewrite_";
    Rcpp::LogicalVector lv = R.parseEval(evalstr + ")");
    Rcpp::List verbose = R.parseEval(evalstr + ", v=T)");
    Rcpp::StringVector errors = verbose["error"];
    
    if (errors.size() > 0 && errors(0) != NA_STRING) {
      *error = errors(0);
    }
    return lv(0);
      
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return false;
}

// Returns the maximum submatch needed for the rewrite to be done by Replace().
// E.g. if rewrite == "foo \\2,\\1", returns 2.
int RE2::MaxSubmatch(const StringPiece& rewrite) {
  try {
    R["rewrite_"] = rewrite.as_string();
    std::string evalstr = "re2_max_submatch(rewrite_)";
    Rcpp::IntegerVector ms = R.parseEval(evalstr);
    return ms(0);
      
  } catch(std::exception& ex) {
    std::cerr << "Exception caught: " << ex.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }
  return -1;
}

// Append the "rewrite" string, with backslash subsitutions from "vec",
// to string "out".
bool RE2::Rewrite(std::string* out,
                  const StringPiece& rewrite,
                  const StringPiece* vec,
                  int veclen) const {
  // Not tested directly by tests in re2/testing
  //  So, don't need any implementation.
  return false;
}

/***** Parsers for various types *****/

namespace re2_internal {

template <>
bool Parse(const char* str, size_t n, void* dest) {
  // We fail if somebody asked us to store into a non-NULL void* pointer
  return (dest == NULL);
}

template <>
bool Parse(const char* str, size_t n, std::string* dest) {
  if (dest == NULL) return true;
  dest->assign(str, n);
  return true;
}

template <>
bool Parse(const char* str, size_t n, StringPiece* dest) {
  if (dest == NULL) return true;
  *dest = StringPiece(str, n);
  return true;
}

template <>
bool Parse(const char* str, size_t n, char* dest) {
  if (n != 1) return false;
  if (dest == NULL) return true;
  *dest = str[0];
  return true;
}

template <>
bool Parse(const char* str, size_t n, signed char* dest) {
  if (n != 1) return false;
  if (dest == NULL) return true;
  *dest = str[0];
  return true;
}

template <>
bool Parse(const char* str, size_t n, unsigned char* dest) {
  if (n != 1) return false;
  if (dest == NULL) return true;
  *dest = str[0];
  return true;
}

// Largest number spec that we are willing to parse
static const int kMaxNumberLength = 32;

// REQUIRES "buf" must have length at least nbuf.
// Copies "str" into "buf" and null-terminates.
// Overwrites *np with the new length.
static const char* TerminateNumber(char* buf, size_t nbuf, const char* str,
                                   size_t* np, bool accept_spaces) {
  size_t n = *np;
  if (n == 0) return "";
  if (n > 0 && isspace(*str)) {
    // We are less forgiving than the strtoxxx() routines and do not
    // allow leading spaces. We do allow leading spaces for floats.
    if (!accept_spaces) {
      return "";
    }
    while (n > 0 && isspace(*str)) {
      n--;
      str++;
    }
  }

  // Although buf has a fixed maximum size, we can still handle
  // arbitrarily large integers correctly by omitting leading zeros.
  // (Numbers that are still too long will be out of range.)
  // Before deciding whether str is too long,
  // remove leading zeros with s/000+/00/.
  // Leaving the leading two zeros in place means that
  // we don't change 0000x123 (invalid) into 0x123 (valid).
  // Skip over leading - before replacing.
  bool neg = false;
  if (n >= 1 && str[0] == '-') {
    neg = true;
    n--;
    str++;
  }

  if (n >= 3 && str[0] == '0' && str[1] == '0') {
    while (n >= 3 && str[2] == '0') {
      n--;
      str++;
    }
  }

  if (neg) {  // make room in buf for -
    n++;
    str--;
  }

  if (n > nbuf-1) return "";

  memmove(buf, str, n);
  if (neg) {
    buf[0] = '-';
  }
  buf[n] = '\0';
  *np = n;
  return buf;
}

template <>
bool Parse(const char* str, size_t n, float* dest) {
  if (n == 0) return false;
  static const int kMaxLength = 200;
  char buf[kMaxLength+1];
  str = TerminateNumber(buf, sizeof buf, str, &n, true);
  char* end;
  errno = 0;
  float r = strtof(str, &end);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  if (dest == NULL) return true;
  *dest = r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, double* dest) {
  if (n == 0) return false;
  static const int kMaxLength = 200;
  char buf[kMaxLength+1];
  str = TerminateNumber(buf, sizeof buf, str, &n, true);
  char* end;
  errno = 0;
  double r = strtod(str, &end);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  if (dest == NULL) return true;
  *dest = r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, long* dest, int radix) {
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, sizeof buf, str, &n, false);
  char* end;
  errno = 0;
  long r = strtol(str, &end, radix);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  if (dest == NULL) return true;
  *dest = r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, unsigned long* dest, int radix) {
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, sizeof buf, str, &n, false);
  if (str[0] == '-') {
    // strtoul() will silently accept negative numbers and parse
    // them.  This module is more strict and treats them as errors.
    return false;
  }

  char* end;
  errno = 0;
  unsigned long r = strtoul(str, &end, radix);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  if (dest == NULL) return true;
  *dest = r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, short* dest, int radix) {
  long r;
  if (!Parse(str, n, &r, radix)) return false;  // Could not parse
  if ((short)r != r) return false;              // Out of range
  if (dest == NULL) return true;
  *dest = (short)r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, unsigned short* dest, int radix) {
  unsigned long r;
  if (!Parse(str, n, &r, radix)) return false;  // Could not parse
  if ((unsigned short)r != r) return false;     // Out of range
  if (dest == NULL) return true;
  *dest = (unsigned short)r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, int* dest, int radix) {
  long r;
  if (!Parse(str, n, &r, radix)) return false;  // Could not parse
  if ((int)r != r) return false;                // Out of range
  if (dest == NULL) return true;
  *dest = (int)r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, unsigned int* dest, int radix) {
  unsigned long r;
  if (!Parse(str, n, &r, radix)) return false;  // Could not parse
  if ((unsigned int)r != r) return false;       // Out of range
  if (dest == NULL) return true;
  *dest = (unsigned int)r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, long long* dest, int radix) {
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, sizeof buf, str, &n, false);
  char* end;
  errno = 0;
  long long r = strtoll(str, &end, radix);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  if (dest == NULL) return true;
  *dest = r;
  return true;
}

template <>
bool Parse(const char* str, size_t n, unsigned long long* dest, int radix) {
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, sizeof buf, str, &n, false);
  if (str[0] == '-') {
    // strtoull() will silently accept negative numbers and parse
    // them.  This module is more strict and treats them as errors.
    return false;
  }
  char* end;
  errno = 0;
  unsigned long long r = strtoull(str, &end, radix);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  if (dest == NULL) return true;
  *dest = r;
  return true;
}

}  // namespace re2_internal

namespace hooks {

#ifdef RE2_HAVE_THREAD_LOCAL
thread_local const RE2* context = NULL;
#endif

template <typename T>
union Hook {
  void Store(T* cb) { cb_.store(cb, std::memory_order_release); }
  T* Load() const { return cb_.load(std::memory_order_acquire); }

#if !defined(__clang__) && defined(_MSC_VER)
  // Citing https://github.com/protocolbuffers/protobuf/pull/4777 as precedent,
  // this is a gross hack to make std::atomic<T*> constant-initialized on MSVC.
  static_assert(ATOMIC_POINTER_LOCK_FREE == 2,
                "std::atomic<T*> must be always lock-free");
  T* cb_for_constinit_;
#endif

  std::atomic<T*> cb_;
};

template <typename T>
static void DoNothing(const T&) {}

#define DEFINE_HOOK(type, name)                                       \
  static Hook<type##Callback> name##_hook = {{&DoNothing<type>}};     \
  void Set##type##Hook(type##Callback* cb) { name##_hook.Store(cb); } \
  type##Callback* Get##type##Hook() { return name##_hook.Load(); }

DEFINE_HOOK(DFAStateCacheReset, dfa_state_cache_reset)
DEFINE_HOOK(DFASearchFailure, dfa_search_failure)

#undef DEFINE_HOOK

}  // namespace hooks

}  // namespace re2
