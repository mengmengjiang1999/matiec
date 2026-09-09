#!/usr/bin/env perl

use strict;
use warnings;

sub replace_flex_fatal_exit {
  my ($path) = @_;
  open my $input, '<', $path or die "$path: $!\n";
  local $/;
  my $content = <$input>;
  close $input or die "$path: $!\n";

  my $replacement = '  throw matiec::CompilationAbort(msg);';
  if (index($content, $replacement) < 0) {
    my @variants = (
      "\t\t\tfprintf( stderr, \"%s\\n\", msg );\n\texit( YY_EXIT_FAILURE );",
      "\t\tfprintf( stderr, \"%s\\n\", msg );\n\texit( YY_EXIT_FAILURE );",
      "\tfprintf( stderr, \"%s\\n\", msg );\n\texit( YY_EXIT_FAILURE );",
    );
    my @matches = grep { index($content, $_) >= 0 } @variants;
    die "$path: expected generated fatal-exit sequence not found\n"
        unless @matches == 1;
    $content =~ s/\Q$matches[0]\E/$replacement/;
  }

  open my $output, '>', $path or die "$path: $!\n";
  print {$output} $content or die "$path: $!\n";
  close $output or die "$path: $!\n";
}

sub reject_process_termination {
  my ($path) = @_;
  open my $input, '<', $path or die "$path: $!\n";
  local $/;
  my $content = <$input>;
  close $input or die "$path: $!\n";
  die "$path: generated frontend still terminates the process\n"
      if $content =~ /\b(?:exit|_Exit|abort)\s*\(/;
}

die "usage: $0 FLEX_CC BISON_CC BISON_HH\n" unless @ARGV == 3;
my ($flex_cc, $bison_cc, $bison_hh) = @ARGV;

replace_flex_fatal_exit($flex_cc);
reject_process_termination($flex_cc);
reject_process_termination($bison_cc);
reject_process_termination($bison_hh);
