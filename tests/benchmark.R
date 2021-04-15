# Author: Girish Palya

#library(microbenchmark)
library(re2)
library(stringr)

REGEXPS <- list(
    # These two are easy because they start with an A,
    # giving the search loop something to memchr for.
    EASY0 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ$",
    EASY1 = "A[AB]B[BC]C[CD]D[DE]E[EF]F[FG]G[GH]H[HI]I[IJ]J$",
    # This is a little harder, since it starts with a character class
    # and thus can't be memchr'ed.  Could look for ABC and work backward,
    # but no one does that.
    MEDIUM = "[XYZ]ABCDEFGHIJKLMNOPQRSTUVWXYZ$",
    # This is a fair amount harder, because of the leading [ -~]*.
    # A bad backtracking implementation will take O(text^2) time to
    # figure out there's no match.
    HARD = "[ -~]*ABCDEFGHIJKLMNOPQRSTUVWXYZ$",
    # This has quite a high degree of fanout.
    # NFA execution will be particularly slow.
    FANOUT = "(?:[\\x{80}-\\x{10FFFF}]?){100}[\\x{80}-\\x{10FFFF}]",
    # This stresses engines that are trying to track parentheses.
    PARENS = paste0("([ -~])*(A)(B)(C)(D)(E)(F)(G)(H)(I)(J)(K)(L)(M)",
                    "(N)(O)(P)(Q)(R)(S)(T)(U)(V)(W)(X)(Y)(Z)$"),
    SUCCESS = ".*$",
    SUCCESS1 = ".*\\C$"
)

# Note: "Elapsed" time is the wall clock time taken to execute the
# function, plus some benchmarking code wrapping it. “User CPU time”
# gives the CPU time spent by the current process (i.e., the current R
# session) and “system CPU time” gives the CPU time spent by the
# kernel (the operating system) on behalf of the current process. The
# operating system is used for things like opening files, doing input
# or output, starting other processes, and looking at the system
# clock: operations that involve resources that many processes must
# share. Different operating systems will have different things done
# by the operating system.  You get seconds from system.time

compile_RE2 <- function(text, pattern, repetition) {
    system.time(replicate(repetition, {
        re2_re2(pattern)               
        NULL
    }))
}

search_cached_RE2 <- function(text, pattern, repetition) {
    re <- re2_re2(pattern)
    system.time(replicate(repetition, {
        stopifnot(re2_match_l(text, re) == FALSE)
        NULL
    }))
}

search_RE2 <- function(text, pattern, repetition) {
    system.time(replicate(repetition, {
        stopifnot(re2_match_l(text, pattern) == FALSE)        
        NULL
    }))
}

search_PCRE <- function(text, pattern, repetition) {
    system.time(replicate(repetition, {
        stopifnot(str_detect(text, pattern) == FALSE)
        NULL
    }))
}

format_bytes <- function(bytes) {
    if (bytes >= bitwShiftL(1, 20)) {
        return(paste0(bytes / bitwShiftL(1, 20), "MB"))
    }
    if (bytes >= bitwShiftL(1, 10)) {
        return(paste0(bytes / bitwShiftL(1, 10), "KB"))        
    }
    return(bytes)
}

search_func <- function(func, fname, text, re_name, regexp) {
    s_time <- func(text, regexp, 1)
    microsec <- s_time["user.self"] * 1e6
    iters <- 1e4
    if (microsec > 1) {
        iters <- round(1e6 / microsec)
    }
    if (iters > 0) { 
        s_time <- func(text, regexp, iters)
        print(paste(fname, re_name,
                    format_bytes(nchar(text)), iters,
                    round((s_time["user.self"] / iters) * 1e6), 'microsec/op'))
        return(invisible(TRUE))
    }
    return(invisible(FALSE))
}

    
run_func <- function(func, fname, text_length, ...) {
    arguments <- list(...)
    re_name = arguments[[1]]
    regexp = arguments[[2]]
    text <- .re2_random_text(text_length)
    return(search_func(func, fname, text, re_name, regexp))
}

apply_func <- function(func, ...) {
    for (shift in 3:24) {
        if (!run_func(func, deparse(substitute(func)), bitwShiftL(1, shift), ...)) {
            break
        }
    }
}

for (re in names(REGEXPS)) {
    apply_func(search_cached_RE2, re, REGEXPS[[re]])
    apply_func(search_RE2, re, REGEXPS[[re]])
    apply_func(search_PCRE, re, REGEXPS[[re]])
}

#run_func(compile_RE2, 8, "Easy1", REGEXPS[["Easy1"]])

