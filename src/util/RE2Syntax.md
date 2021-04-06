# Black Lives Matter. [Support the Equal Justice Initiative.](https://support.eji.org/give/153413/#!/donation/checkout)

<p>
Regular expressions are a notation for
describing sets of character strings.
When a particular string is in the set
described by a regular expression,
we often say that the regular expression
<i>matches</i>
the string.
</p>

<p>
The simplest regular expression is a single literal character.
Except for the metacharacters like
<code>*+?()|</code>,
characters match themselves.
To match a metacharacter, escape it with
a backslash:
<code>\+</code>
matches a literal plus character.
</p>

<p>
Two regular expressions can be alternated or concatenated to form a new
regular expression:
if <i>e</i><sub>1</sub> matches
<i>s</i>
and <i>e</i><sub>2</sub> matches
<i>t</i>,
then <i>e</i><sub>1</sub><code>|</code><i>e</i><sub>2</sub> matches
<i>s</i>
or
<i>t</i>,
and
<i>e</i><sub>1</sub><i>e</i><sub>2</sub>
matches
<i>st</i>.
</p>

<p>
The metacharacters
<code>*</code>,
<code>+</code>,
and
<code>?</code>
are repetition operators:
<i>e</i><sub>1</sub><code>*</code>
matches a sequence of zero or more (possibly different)
strings, each of which match <i>e</i><sub>1</sub>;
<i>e</i><sub>1</sub><code>+</code>
matches one or more;
<i>e</i><sub>1</sub><code>?</code>
matches zero or one.
</p>

<p>
The operator precedence, from weakest to strongest binding, is
first alternation, then concatenation, and finally the
repetition operators.
Explicit parentheses can be used to force different meanings,
just as in arithmetic expressions.
Some examples:
<code>ab|cd</code>
is equivalent to
<code>(ab)|(cd)</code>;
<code>ab*</code>
is equivalent to
<code>a(b*)</code>.
</p>

<p>
The syntax described so far is most of the traditional Unix
<i>egrep</i>
regular expression syntax.
This subset suffices to describe all regular
languages: loosely speaking, a regular language is a set
of strings that can be matched in a single pass through
the text using only a fixed amount of memory.
Newer regular expression facilities (notably Perl and
those that have copied it) have added
many new operators and escape sequences, which make the regular
expressions more concise, and sometimes more cryptic, but usually
not more powerful.
</p>

<p>This page lists the regular expression syntax accepted by RE2.
<p>It also lists some syntax accepted by PCRE, PERL, and VIM.

<table>
<tr><th>kinds of single-character expressions</th><th>examples</th></tr>
<tr><td>any character, possibly including newline (s=true)</td><td><code>.</code></td></tr>
<tr><td>character class</td><td><code>[xyz]</code></td></tr>
<tr><td>negated character class</td><td><code>[^xyz]</code></td></tr>
<tr><td>Perl character class <a href="#perl">(link)</a></td><td><code>\d</code></td></tr>
<tr><td>negated Perl character class</td><td><code>\D</code></td></tr>
<tr><td>ASCII character class <a href="#ascii">(link)</a></td><td><code>[[:alpha:]]</code></td></tr>
<tr><td>negated ASCII character class</td><td><code>[[:^alpha:]]</code></td></tr>
<tr><td>Unicode character class (one-letter name)</td><td><code>\pN</code></td></tr>
<tr><td>Unicode character class</td><td><code>\p{Greek}</code></td></tr>
<tr><td>negated Unicode character class (one-letter name)</td><td><code>\PN</code></td></tr>
<tr><td>negated Unicode character class</td><td><code>\P{Greek}</code></td></tr>
</table>

<table>
<tr><th></th><th>Composites</th></tr>
<tr><td><code>xy</code></td><td><code>x</code> followed by <code>y</code></td></tr>
<tr><td><code>x|y</code></td><td><code>x</code> or <code>y</code> (prefer <code>x</code>)</td></tr>
</table>

<table>
<tr><th></th><th>Repetitions</th></tr>
<tr><td><code>x*</code></td><td>zero or more <code>x</code>, prefer more</td></tr>
<tr><td><code>x+</code></td><td>one or more <code>x</code>, prefer more</td></tr>
<tr><td><code>x?</code></td><td>zero or one <code>x</code>, prefer one</td></tr>
<tr><td><code>x{n,m}</code></td><td><code>n</code> or <code>n</code>+1 or ... or <code>m</code> <code>x</code>, prefer more</td></tr>
<tr><td><code>x{n,}</code></td><td><code>n</code> or more <code>x</code>, prefer more</td></tr>
<tr><td><code>x{n}</code></td><td>exactly <code>n</code> <code>x</code></td></tr>
<tr><td><code>x*?</code></td><td>zero or more <code>x</code>, prefer fewer</td></tr>
<tr><td><code>x+?</code></td><td>one or more <code>x</code>, prefer fewer</td></tr>
<tr><td><code>x??</code></td><td>zero or one <code>x</code>, prefer zero</td></tr>
<tr><td><code>x{n,m}?</code></td><td><code>n</code> or <code>n</code>+1 or ... or <code>m</code> <code>x</code>, prefer fewer</td></tr>
<tr><td><code>x{n,}?</code></td><td><code>n</code> or more <code>x</code>, prefer fewer</td></tr>
<tr><td><code>x{n}?</code></td><td>exactly <code>n</code> <code>x</code></td></tr>
<tr><td><font color='#808080'><code>x{}</code></font></td><td>(≡ <code>x*</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>x{-}</code></font></td><td>(≡ <code>x*?</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>x{-n}</code></font></td><td>(≡ <code>x{n}?</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>x=</code></font></td><td>(≡ <code>x?</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
</table>

<p>Implementation restriction: The counting forms <code>x{n,m}</code>, <code>x{n,}</code>, and <code>x{n}</code> reject forms that create a minimum or maximum repetition count above 1000. Unlimited repetitions are not subject to this restriction.

<table>
<tr><th></th><th>Possessive repetitions</th></tr>
<tr><td><font color='#808080'><code>x*+</code></font></td><td>zero or more <code>x</code>, possessive <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>x++</code></font></td><td>one or more <code>x</code>, possessive <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>x?+</code></font></td><td>zero or one <code>x</code>, possessive <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>x{n,m}+</code></font></td><td><code>n</code> or ... or <code>m</code> <code>x</code>, possessive <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>x{n,}+</code></font></td><td><code>n</code> or more <code>x</code>, possessive <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>x{n}+</code></font></td><td>exactly <code>n</code> <code>x</code>, possessive <small>(NOT SUPPORTED)</small></td></tr>
</table>

<table>
<tr><th></th><th>Grouping</th></tr>
<tr><td><code>(re)</code></td><td>numbered capturing group (submatch)</td></tr>
<tr><td><code>(?P&lt;name&gt;re)</code></td><td>named & numbered capturing group (submatch)</td></tr>
<tr><td><font color='#808080'><code>(?&lt;name&gt;re)</code></font></td><td>named & numbered capturing group (submatch) <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?'name're)</code></font></td><td>named & numbered capturing group (submatch) <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><code>(?:re)</code></td><td>non-capturing group</td></tr>
<tr><td><code>(?flags)</code></td><td>set flags within current group; non-capturing</td></tr>
<tr><td><code>(?flags:re)</code></td><td>set flags during re; non-capturing</td></tr>
<tr><td><font color='#808080'><code>(?#text)</code></font></td><td>comment <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?|x|y|z)</code></font></td><td>branch numbering reset <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?&gt;re)</code></font></td><td>possessive match of <code>re</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>re@&gt;</code></font></td><td>possessive match of <code>re</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>%(re)</code></font></td><td>non-capturing group <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
</table>

<a name="flags"></a>
<table>
<tr><th></th><th>Flags</th></tr>
<tr><td><code>i</code></td><td>case-insensitive (default false)</td></tr>
<tr><td><code>m</code></td><td>multi-line mode: <code>^</code> and <code>$</code> match begin/end line in addition to begin/end text (default false)</td></tr>
<tr><td><code>s</code></td><td>let <code>.</code> match <code>\n</code> (default false)</td></tr>
<tr><td><code>U</code></td><td>ungreedy: swap meaning of <code>x*</code> and <code>x*?</code>, <code>x+</code> and <code>x+?</code>, etc (default false)</td></tr>
</table>

<p>Flag syntax is <code>xyz</code> (set) or <code>-xyz</code> (clear) or <code>xy-z</code> (set <code>xy</code>, clear <code>z</code>).

<table>
<tr><th></th><th>Empty strings</th></tr>
<tr><td><code>^</code></td><td>at beginning of text or line (<code>m</code>=true)</td></tr>
<tr><td><code>$</code></td><td>at end of text (like <code>\z</code> not <code>\Z</code>) or line (<code>m</code>=true)</td></tr>
<tr><td><code>\A</code></td><td>at beginning of text</td></tr>
<tr><td><code>\b</code></td><td>at ASCII word boundary (<code>\w</code> on one side and <code>\W</code>, <code>\A</code>, or <code>\z</code> on the other)</td></tr>
<tr><td><code>\B</code></td><td>not at ASCII word boundary</td></tr>
<tr><td><font color='#808080'><code>\g</code></font></td><td>at beginning of subtext being searched <small>(NOT SUPPORTED)</small> <small>PCRE</small></td></tr>
<tr><td><font color='#808080'><code>\G</code></font></td><td>at end of last match <small>(NOT SUPPORTED)</small> <small>PERL</small></td></tr>
<tr><td><font color='#808080'><code>\Z</code></font></td><td>at end of text, or before newline at end of text <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><code>\z</code></td><td>at end of text</td></tr>
<tr><td><font color='#808080'><code>(?=re)</code></font></td><td>before text matching <code>re</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?!re)</code></font></td><td>before text not matching <code>re</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?&lt;=re)</code></font></td><td>after text matching <code>re</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?&lt;!re)</code></font></td><td>after text not matching <code>re</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>re&amp;</code></font></td><td>before text matching <code>re</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>re@=</code></font></td><td>before text matching <code>re</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>re@!</code></font></td><td>before text not matching <code>re</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>re@&lt;=</code></font></td><td>after text matching <code>re</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>re@&lt;!</code></font></td><td>after text not matching <code>re</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\zs</code></font></td><td>sets start of match (= \K) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\ze</code></font></td><td>sets end of match <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%^</code></font></td><td>beginning of file <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%$</code></font></td><td>end of file <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%V</code></font></td><td>on screen <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%#</code></font></td><td>cursor position <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%'m</code></font></td><td>mark <code>m</code> position <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%23l</code></font></td><td>in line 23 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%23c</code></font></td><td>in column 23 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%23v</code></font></td><td>in virtual column 23 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
</table>

<table>
<tr><th></th><th>Escape sequences</th></tr>
<tr><td><code>\a</code></td><td>bell (≡ <code>\007</code>)</td></tr>
<tr><td><code>\f</code></td><td>form feed (≡ <code>\014</code>)</td></tr>
<tr><td><code>\t</code></td><td>horizontal tab (≡ <code>\011</code>)</td></tr>
<tr><td><code>\n</code></td><td>newline (≡ <code>\012</code>)</td></tr>
<tr><td><code>\r</code></td><td>carriage return (≡ <code>\015</code>)</td></tr>
<tr><td><code>\v</code></td><td>vertical tab character (≡ <code>\013</code>)</td></tr>
<tr><td><code>\*</code></td><td>literal <code>*</code>, for any punctuation character <code>*</code></td></tr>
<tr><td><code>\123</code></td><td>octal character code (up to three digits)</td></tr>
<tr><td><code>\x7F</code></td><td>hex character code (exactly two digits)</td></tr>
<tr><td><code>\x{10FFFF}</code></td><td>hex character code</td></tr>
<tr><td><code>\C</code></td><td>match a single byte even in UTF-8 mode</td></tr>
<tr><td><code>\Q...\E</code></td><td>literal text <code>...</code> even if <code>...</code> has punctuation</td></tr>
<tr><td><font color='#808080'><code>\1</code></font></td><td>backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\b</code></font></td><td>backspace <small>(NOT SUPPORTED)</small> (use <code>\010</code>)</td></tr>
<tr><td><font color='#808080'><code>\cK</code></font></td><td>control char ^K <small>(NOT SUPPORTED)</small> (use <code>\001</code> etc)</td></tr>
<tr><td><font color='#808080'><code>\e</code></font></td><td>escape <small>(NOT SUPPORTED)</small> (use <code>\033</code>)</td></tr>
<tr><td><font color='#808080'><code>\g1</code></font></td><td>backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\g{1}</code></font></td><td>backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\g{+1}</code></font></td><td>backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\g{-1}</code></font></td><td>backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\g{name}</code></font></td><td>named backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\g&lt;name&gt;</code></font></td><td>subroutine call <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\g'name'</code></font></td><td>subroutine call <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\k&lt;name&gt;</code></font></td><td>named backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\k'name'</code></font></td><td>named backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\lX</code></font></td><td>lowercase <code>X</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\ux</code></font></td><td>uppercase <code>x</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\L...\E</code></font></td><td>lowercase text <code>...</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\K</code></font></td><td>reset beginning of <code>$0</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\N{name}</code></font></td><td>named Unicode character <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\R</code></font></td><td>line break <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\U...\E</code></font></td><td>upper case text <code>...</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\X</code></font></td><td>extended Unicode sequence <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\%d123</code></font></td><td>decimal character 123 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%xFF</code></font></td><td>hex character FF <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%o123</code></font></td><td>octal character 123 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%u1234</code></font></td><td>Unicode character 0x1234 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\%U12345678</code></font></td><td>Unicode character 0x12345678 <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
</table>

<table>
<tr><th></th><th>Character class elements</th></tr>
<tr><td><code>x</code></td><td>single character</td></tr>
<tr><td><code>A-Z</code></td><td>character range (inclusive)</td></tr>
<tr><td><code>\d</code></td><td>Perl character class</td></tr>
<tr><td><code>[:foo:]</code></td><td>ASCII character class <code>foo</code></td></tr>
<tr><td><code>\p{Foo}</code></td><td>Unicode character class <code>Foo</code></td></tr>
<tr><td><code>\pF</code></td><td>Unicode character class <code>F</code> (one-letter name)</td></tr>
</table>

<table>
<tr><th></th><th>Named character classes as character class elements</th></tr>
<tr><td><code>[\d]</code></td><td>digits (≡ <code>\d</code>)</td></tr>
<tr><td><code>[^\d]</code></td><td>not digits (≡ <code>\D</code>)</td></tr>
<tr><td><code>[\D]</code></td><td>not digits (≡ <code>\D</code>)</td></tr>
<tr><td><code>[^\D]</code></td><td>not not digits (≡ <code>\d</code>)</td></tr>
<tr><td><code>[[:name:]]</code></td><td>named ASCII class inside character class (≡ <code>[:name:]</code>)</td></tr>
<tr><td><code>[^[:name:]]</code></td><td>named ASCII class inside negated character class (≡ <code>[:^name:]</code>)</td></tr>
<tr><td><code>[\p{Name}]</code></td><td>named Unicode property inside character class (≡ <code>\p{Name}</code>)</td></tr>
<tr><td><code>[^\p{Name}]</code></td><td>named Unicode property inside negated character class (≡ <code>\P{Name}</code>)</td></tr>
</table>

<a name="perl"></a>
<table>
<tr><th></th><th>Perl character classes (all ASCII-only)</th></tr>
<tr><td><code>\d</code></td><td>digits (≡ <code>[0-9]</code>)</td></tr>
<tr><td><code>\D</code></td><td>not digits (≡ <code>[^0-9]</code>)</td></tr>
<tr><td><code>\s</code></td><td>whitespace (≡ <code>[\t\n\f\r ]</code>)</td></tr>
<tr><td><code>\S</code></td><td>not whitespace (≡ <code>[^\t\n\f\r ]</code>)</td></tr>
<tr><td><code>\w</code></td><td>word characters (≡ <code>[0-9A-Za-z_]</code>)</td></tr>
<tr><td><code>\W</code></td><td>not word characters (≡ <code>[^0-9A-Za-z_]</code>)</td></tr>
<tr><td><font color='#808080'><code>\h</code></font></td><td>horizontal space <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\H</code></font></td><td>not horizontal space <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\v</code></font></td><td>vertical space <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>\V</code></font></td><td>not vertical space <small>(NOT SUPPORTED)</small></td></tr>
</table>

<a name="ascii"></a>
<table>
<tr><th></th><th>ASCII character classes</th></tr>
<tr><td><code>[[:alnum:]]</code></td><td>alphanumeric (≡ <code>[0-9A-Za-z]</code>)</td></tr>
<tr><td><code>[[:alpha:]]</code></td><td>alphabetic (≡ <code>[A-Za-z]</code>)</td></tr>
<tr><td><code>[[:ascii:]]</code></td><td>ASCII (≡ <code>[\x00-\x7F]</code>)</td></tr>
<tr><td><code>[[:blank:]]</code></td><td>blank (≡ <code>[\t ]</code>)</td></tr>
<tr><td><code>[[:cntrl:]]</code></td><td>control (≡ <code>[\x00-\x1F\x7F]</code>)</td></tr>
<tr><td><code>[[:digit:]]</code></td><td>digits (≡ <code>[0-9]</code>)</td></tr>
<tr><td><code>[[:graph:]]</code></td><td>graphical (≡ <code>[!-~]</code> ≡ <code>[A-Za-z0-9!"#$%&amp;'()*+,\-./:;&lt;=&gt;?@[\\\]^_</code><code>`</code><code>{|}~]</code>)</td></tr>
<tr><td><code>[[:lower:]]</code></td><td>lower case (≡ <code>[a-z]</code>)</td></tr>
<tr><td><code>[[:print:]]</code></td><td>printable (≡ <code>[ -~]</code> ≡ <code>[ [:graph:]]</code>)</td></tr>
<tr><td><code>[[:punct:]]</code></td><td>punctuation (≡ <code>[!-/:-@[-</code><code>`</code><code>{-~]</code>)</td></tr>
<tr><td><code>[[:space:]]</code></td><td>whitespace (≡ <code>[\t\n\v\f\r ]</code>)</td></tr>
<tr><td><code>[[:upper:]]</code></td><td>upper case (≡ <code>[A-Z]</code>)</td></tr>
<tr><td><code>[[:word:]]</code></td><td>word characters (≡ <code>[0-9A-Za-z_]</code>)</td></tr>
<tr><td><code>[[:xdigit:]]</code></td><td>hex digit (≡ <code>[0-9A-Fa-f]</code>)</td></tr>
</table>

<table>
<tr><th></th><th>Unicode character class names--general category</th></tr>
<tr><td><code>C</code></td><td>other</td></tr>
<tr><td><code>Cc</code></td><td>control</td></tr>
<tr><td><code>Cf</code></td><td>format</td></tr>
<tr><td><font color='#808080'><code>Cn</code></font></td><td>unassigned code points <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><code>Co</code></td><td>private use</td></tr>
<tr><td><code>Cs</code></td><td>surrogate</td></tr>
<tr><td><code>L</code></td><td>letter</td></tr>
<tr><td><font color='#808080'><code>LC</code></font></td><td>cased letter <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>L&amp;</code></font></td><td>cased letter <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><code>Ll</code></td><td>lowercase letter</td></tr>
<tr><td><code>Lm</code></td><td>modifier letter</td></tr>
<tr><td><code>Lo</code></td><td>other letter</td></tr>
<tr><td><code>Lt</code></td><td>titlecase letter</td></tr>
<tr><td><code>Lu</code></td><td>uppercase letter</td></tr>
<tr><td><code>M</code></td><td>mark</td></tr>
<tr><td><code>Mc</code></td><td>spacing mark</td></tr>
<tr><td><code>Me</code></td><td>enclosing mark</td></tr>
<tr><td><code>Mn</code></td><td>non-spacing mark</td></tr>
<tr><td><code>N</code></td><td>number</td></tr>
<tr><td><code>Nd</code></td><td>decimal number</td></tr>
<tr><td><code>Nl</code></td><td>letter number</td></tr>
<tr><td><code>No</code></td><td>other number</td></tr>
<tr><td><code>P</code></td><td>punctuation</td></tr>
<tr><td><code>Pc</code></td><td>connector punctuation</td></tr>
<tr><td><code>Pd</code></td><td>dash punctuation</td></tr>
<tr><td><code>Pe</code></td><td>close punctuation</td></tr>
<tr><td><code>Pf</code></td><td>final punctuation</td></tr>
<tr><td><code>Pi</code></td><td>initial punctuation</td></tr>
<tr><td><code>Po</code></td><td>other punctuation</td></tr>
<tr><td><code>Ps</code></td><td>open punctuation</td></tr>
<tr><td><code>S</code></td><td>symbol</td></tr>
<tr><td><code>Sc</code></td><td>currency symbol</td></tr>
<tr><td><code>Sk</code></td><td>modifier symbol</td></tr>
<tr><td><code>Sm</code></td><td>math symbol</td></tr>
<tr><td><code>So</code></td><td>other symbol</td></tr>
<tr><td><code>Z</code></td><td>separator</td></tr>
<tr><td><code>Zl</code></td><td>line separator</td></tr>
<tr><td><code>Zp</code></td><td>paragraph separator</td></tr>
<tr><td><code>Zs</code></td><td>space separator</td></tr>
</table>

<table>
<tr><th>Unicode character class names--scripts</th></tr>
<tr><td><code>Adlam</code></td></tr>
<tr><td><code>Ahom</code></td></tr>
<tr><td><code>Anatolian_Hieroglyphs</code></td></tr>
<tr><td><code>Arabic</code></td></tr>
<tr><td><code>Armenian</code></td></tr>
<tr><td><code>Avestan</code></td></tr>
<tr><td><code>Balinese</code></td></tr>
<tr><td><code>Bamum</code></td></tr>
<tr><td><code>Bassa_Vah</code></td></tr>
<tr><td><code>Batak</code></td></tr>
<tr><td><code>Bengali</code></td></tr>
<tr><td><code>Bhaiksuki</code></td></tr>
<tr><td><code>Bopomofo</code></td></tr>
<tr><td><code>Brahmi</code></td></tr>
<tr><td><code>Braille</code></td></tr>
<tr><td><code>Buginese</code></td></tr>
<tr><td><code>Buhid</code></td></tr>
<tr><td><code>Canadian_Aboriginal</code></td></tr>
<tr><td><code>Carian</code></td></tr>
<tr><td><code>Caucasian_Albanian</code></td></tr>
<tr><td><code>Chakma</code></td></tr>
<tr><td><code>Cham</code></td></tr>
<tr><td><code>Cherokee</code></td></tr>
<tr><td><code>Chorasmian</code></td></tr>
<tr><td><code>Common</code></td></tr>
<tr><td><code>Coptic</code></td></tr>
<tr><td><code>Cuneiform</code></td></tr>
<tr><td><code>Cypriot</code></td></tr>
<tr><td><code>Cyrillic</code></td></tr>
<tr><td><code>Deseret</code></td></tr>
<tr><td><code>Devanagari</code></td></tr>
<tr><td><code>Dives_Akuru</code></td></tr>
<tr><td><code>Dogra</code></td></tr>
<tr><td><code>Duployan</code></td></tr>
<tr><td><code>Egyptian_Hieroglyphs</code></td></tr>
<tr><td><code>Elbasan</code></td></tr>
<tr><td><code>Elymaic</code></td></tr>
<tr><td><code>Ethiopic</code></td></tr>
<tr><td><code>Georgian</code></td></tr>
<tr><td><code>Glagolitic</code></td></tr>
<tr><td><code>Gothic</code></td></tr>
<tr><td><code>Grantha</code></td></tr>
<tr><td><code>Greek</code></td></tr>
<tr><td><code>Gujarati</code></td></tr>
<tr><td><code>Gunjala_Gondi</code></td></tr>
<tr><td><code>Gurmukhi</code></td></tr>
<tr><td><code>Han</code></td></tr>
<tr><td><code>Hangul</code></td></tr>
<tr><td><code>Hanifi_Rohingya</code></td></tr>
<tr><td><code>Hanunoo</code></td></tr>
<tr><td><code>Hatran</code></td></tr>
<tr><td><code>Hebrew</code></td></tr>
<tr><td><code>Hiragana</code></td></tr>
<tr><td><code>Imperial_Aramaic</code></td></tr>
<tr><td><code>Inherited</code></td></tr>
<tr><td><code>Inscriptional_Pahlavi</code></td></tr>
<tr><td><code>Inscriptional_Parthian</code></td></tr>
<tr><td><code>Javanese</code></td></tr>
<tr><td><code>Kaithi</code></td></tr>
<tr><td><code>Kannada</code></td></tr>
<tr><td><code>Katakana</code></td></tr>
<tr><td><code>Kayah_Li</code></td></tr>
<tr><td><code>Kharoshthi</code></td></tr>
<tr><td><code>Khitan_Small_Script</code></td></tr>
<tr><td><code>Khmer</code></td></tr>
<tr><td><code>Khojki</code></td></tr>
<tr><td><code>Khudawadi</code></td></tr>
<tr><td><code>Lao</code></td></tr>
<tr><td><code>Latin</code></td></tr>
<tr><td><code>Lepcha</code></td></tr>
<tr><td><code>Limbu</code></td></tr>
<tr><td><code>Linear_A</code></td></tr>
<tr><td><code>Linear_B</code></td></tr>
<tr><td><code>Lisu</code></td></tr>
<tr><td><code>Lycian</code></td></tr>
<tr><td><code>Lydian</code></td></tr>
<tr><td><code>Mahajani</code></td></tr>
<tr><td><code>Makasar</code></td></tr>
<tr><td><code>Malayalam</code></td></tr>
<tr><td><code>Mandaic</code></td></tr>
<tr><td><code>Manichaean</code></td></tr>
<tr><td><code>Marchen</code></td></tr>
<tr><td><code>Masaram_Gondi</code></td></tr>
<tr><td><code>Medefaidrin</code></td></tr>
<tr><td><code>Meetei_Mayek</code></td></tr>
<tr><td><code>Mende_Kikakui</code></td></tr>
<tr><td><code>Meroitic_Cursive</code></td></tr>
<tr><td><code>Meroitic_Hieroglyphs</code></td></tr>
<tr><td><code>Miao</code></td></tr>
<tr><td><code>Modi</code></td></tr>
<tr><td><code>Mongolian</code></td></tr>
<tr><td><code>Mro</code></td></tr>
<tr><td><code>Multani</code></td></tr>
<tr><td><code>Myanmar</code></td></tr>
<tr><td><code>Nabataean</code></td></tr>
<tr><td><code>Nandinagari</code></td></tr>
<tr><td><code>New_Tai_Lue</code></td></tr>
<tr><td><code>Newa</code></td></tr>
<tr><td><code>Nko</code></td></tr>
<tr><td><code>Nushu</code></td></tr>
<tr><td><code>Nyiakeng_Puachue_Hmong</code></td></tr>
<tr><td><code>Ogham</code></td></tr>
<tr><td><code>Ol_Chiki</code></td></tr>
<tr><td><code>Old_Hungarian</code></td></tr>
<tr><td><code>Old_Italic</code></td></tr>
<tr><td><code>Old_North_Arabian</code></td></tr>
<tr><td><code>Old_Permic</code></td></tr>
<tr><td><code>Old_Persian</code></td></tr>
<tr><td><code>Old_Sogdian</code></td></tr>
<tr><td><code>Old_South_Arabian</code></td></tr>
<tr><td><code>Old_Turkic</code></td></tr>
<tr><td><code>Oriya</code></td></tr>
<tr><td><code>Osage</code></td></tr>
<tr><td><code>Osmanya</code></td></tr>
<tr><td><code>Pahawh_Hmong</code></td></tr>
<tr><td><code>Palmyrene</code></td></tr>
<tr><td><code>Pau_Cin_Hau</code></td></tr>
<tr><td><code>Phags_Pa</code></td></tr>
<tr><td><code>Phoenician</code></td></tr>
<tr><td><code>Psalter_Pahlavi</code></td></tr>
<tr><td><code>Rejang</code></td></tr>
<tr><td><code>Runic</code></td></tr>
<tr><td><code>Samaritan</code></td></tr>
<tr><td><code>Saurashtra</code></td></tr>
<tr><td><code>Sharada</code></td></tr>
<tr><td><code>Shavian</code></td></tr>
<tr><td><code>Siddham</code></td></tr>
<tr><td><code>SignWriting</code></td></tr>
<tr><td><code>Sinhala</code></td></tr>
<tr><td><code>Sogdian</code></td></tr>
<tr><td><code>Sora_Sompeng</code></td></tr>
<tr><td><code>Soyombo</code></td></tr>
<tr><td><code>Sundanese</code></td></tr>
<tr><td><code>Syloti_Nagri</code></td></tr>
<tr><td><code>Syriac</code></td></tr>
<tr><td><code>Tagalog</code></td></tr>
<tr><td><code>Tagbanwa</code></td></tr>
<tr><td><code>Tai_Le</code></td></tr>
<tr><td><code>Tai_Tham</code></td></tr>
<tr><td><code>Tai_Viet</code></td></tr>
<tr><td><code>Takri</code></td></tr>
<tr><td><code>Tamil</code></td></tr>
<tr><td><code>Tangut</code></td></tr>
<tr><td><code>Telugu</code></td></tr>
<tr><td><code>Thaana</code></td></tr>
<tr><td><code>Thai</code></td></tr>
<tr><td><code>Tibetan</code></td></tr>
<tr><td><code>Tifinagh</code></td></tr>
<tr><td><code>Tirhuta</code></td></tr>
<tr><td><code>Ugaritic</code></td></tr>
<tr><td><code>Vai</code></td></tr>
<tr><td><code>Wancho</code></td></tr>
<tr><td><code>Warang_Citi</code></td></tr>
<tr><td><code>Yezidi</code></td></tr>
<tr><td><code>Yi</code></td></tr>
<tr><td><code>Zanabazar_Square</code></td></tr>
</table>

<table>
<tr><th></th><th>Vim character classes</th></tr>
<tr><td><font color='#808080'><code>\i</code></font></td><td>identifier character <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\I</code></font></td><td><code>\i</code> except digits <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\k</code></font></td><td>keyword character <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\K</code></font></td><td><code>\k</code> except digits <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\f</code></font></td><td>file name character <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\F</code></font></td><td><code>\f</code> except digits <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\p</code></font></td><td>printable character <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\P</code></font></td><td><code>\p</code> except digits <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\s</code></font></td><td>whitespace character (≡ <code>[ \t]</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\S</code></font></td><td>non-white space character (≡ <code>[^ \t]</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><code>\d</code></td><td>digits (≡ <code>[0-9]</code>) <small>VIM</small></td></tr>
<tr><td><code>\D</code></td><td>not <code>\d</code> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\x</code></font></td><td>hex digits (≡ <code>[0-9A-Fa-f]</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\X</code></font></td><td>not <code>\x</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\o</code></font></td><td>octal digits (≡ <code>[0-7]</code>) <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\O</code></font></td><td>not <code>\o</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><code>\w</code></td><td>word character <small>VIM</small></td></tr>
<tr><td><code>\W</code></td><td>not <code>\w</code> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\h</code></font></td><td>head of word character <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\H</code></font></td><td>not <code>\h</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\a</code></font></td><td>alphabetic <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\A</code></font></td><td>not <code>\a</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\l</code></font></td><td>lowercase <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\L</code></font></td><td>not lowercase <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\u</code></font></td><td>uppercase <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\U</code></font></td><td>not uppercase <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\_x</code></font></td><td><code>\x</code> plus newline, for any <code>x</code> <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\c</code></font></td><td>ignore case <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\C</code></font></td><td>match case <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\m</code></font></td><td>magic <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\M</code></font></td><td>nomagic <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\v</code></font></td><td>verymagic <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\V</code></font></td><td>verynomagic <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
<tr><td><font color='#808080'><code>\Z</code></font></td><td>ignore differences in Unicode combining characters <small>(NOT SUPPORTED)</small> <small>VIM</small></td></tr>
</table>

<table>
<tr><th></th><th>Magic</th></tr>
<tr><td><font color='#808080'><code>(?{code})</code></font></td><td>arbitrary Perl code <small>(NOT SUPPORTED)</small> <small>PERL</small></td></tr>
<tr><td><font color='#808080'><code>(??{code})</code></font></td><td>postponed arbitrary Perl code <small>(NOT SUPPORTED)</small> <small>PERL</small></td></tr>
<tr><td><font color='#808080'><code>(?n)</code></font></td><td>recursive call to regexp capturing group <code>n</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?+n)</code></font></td><td>recursive call to relative group <code>+n</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?-n)</code></font></td><td>recursive call to relative group <code>-n</code> <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?C)</code></font></td><td>PCRE callout <small>(NOT SUPPORTED)</small> <small>PCRE</small></td></tr>
<tr><td><font color='#808080'><code>(?R)</code></font></td><td>recursive call to entire regexp (≡ <code>(?0)</code>) <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?&amp;name)</code></font></td><td>recursive call to named group <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?P=name)</code></font></td><td>named backreference <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?P&gt;name)</code></font></td><td>recursive call to named group <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?(cond)true|false)</code></font></td><td>conditional branch <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(?(cond)true)</code></font></td><td>conditional branch <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*ACCEPT)</code></font></td><td>make regexps more like Prolog <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*COMMIT)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*F)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*FAIL)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*MARK)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*PRUNE)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*SKIP)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*THEN)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*ANY)</code></font></td><td>set newline convention <small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*ANYCRLF)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*CR)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*CRLF)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*LF)</code></font></td><td><small>(NOT SUPPORTED)</small></td></tr>
<tr><td><font color='#808080'><code>(*BSR_ANYCRLF)</code></font></td><td>set \R convention <small>(NOT SUPPORTED)</small> <small>PCRE</small></td></tr>
<tr><td><font color='#808080'><code>(*BSR_UNICODE)</code></font></td><td><small>(NOT SUPPORTED)</small> <small>PCRE</small></td></tr>
</table>
