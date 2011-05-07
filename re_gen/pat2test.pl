#!/usr/bin/perl -w

#
# pat2test expects tab-separated pattern-text pairs, one per line, on STDIN.
#

use String::ShellQuote qw(shell_quote);

# print head stuff
print <<HEAD;
#include <scope/test.h>

#include "nfabuilder.h"
#include "parsetree.h"

#include "stest.h"

HEAD

my $patnum = 0;

# read the patterns
while (<>) {
  chomp;
  my ($pat, $text) = split /\t/, $_, 2;

  # write the text to a temporary file
  $text_file = 'text.tmp';
  !-f $text_file or die "error: $text_file already exists!\n";
  open(TEXT, ">$text_file") or die "$!\n";
  print TEXT $text;
  close TEXT;

  # make sure that the pattern is properly quoted
  $qpat = shell_quote($pat);

  # get matches from shitgrep
  system("./shitgrep -p $qpat $text_file 1>sg.stdout 2>sg.stderr");

  open(SGERR, '<sg.stderr') or die "$!\n";
  my $sgerr = join '', <SGERR>;
  my $zero = $sgerr =~ /state \d+ is not allowed as a final state of the NFA/;
  close SGERR;
  
  if ($zero) {
    # test this pattern for zero-length matches
    print <<TEST;
SCOPE_TEST(autoPatternTest$patnum) {
  NFABuilder nfab;
  ParseTree tree;
  SCOPE_ASSERT(parse("$pat", false, tree));
  SCOPE_ASSERT(!nfab.build(tree));
}

TEST
  }
  else {
    # this pattern has no zero-length matches
    open(SGOUT, '<sg.stdout') or die "$!\n";
    my @sgout = <SGOUT>;
    close SGOUT;

    my @matches = ();
    foreach $m (@sgout) {
      push(@matches, [(split('\t', $m))[0..2]]);
    }

    # print the scope test for this pattern and text
    print <<TEST;
SCOPE_FIXTURE_CTOR(autoPatternTest$patnum, STest, STest("$pat")) {
  const byte* text = (const byte*) "$text";
  fixture.search(text, text + @{[ length $text ]}, 0, fixture);
  SCOPE_ASSERT_EQUAL(@{[ $#matches + 1 ]}, fixture.Hits.size());
TEST

    for ($i = 0; $i <= $#matches; ++$i) {
      printf(
        "  SCOPE_ASSERT_EQUAL(SearchHit(%d, %d, %d), fixture.Hits[%d]);\n",
        $matches[$i]->[0], $matches[$i]->[1], $matches[$i]->[2], $i
      );
    }
  
    print "}\n\n";
  }

  ++$patnum;

  unlink $text_file or die "$!\n";
}

# cleanup
unlink 'sg.stdout' or die "$!\n";
unlink 'sg.stderr' or die "$!\n";