#!/usr/bin/env perl

use strict;
use warnings;

sub qualify_once {
  my ($content_ref, $path, $plain, $qualified) = @_;
  return if index($$content_ref, $qualified) >= 0;

  my $first = index($$content_ref, $plain);
  die "$path: expected generated declaration not found: $plain\n"
      if $first < 0;
  die "$path: generated declaration is not unique: $plain\n"
      if index($$content_ref, $plain, $first + length($plain)) >= 0;
  substr($$content_ref, $first, length($plain), $qualified);
}

sub qualify_variant {
  my ($content_ref, $path, $variants) = @_;
  for my $variant (@$variants) {
    return if index($$content_ref, $variant->[1]) >= 0;
  }

  my @matches = grep { index($$content_ref, $_->[0]) >= 0 } @$variants;
  die "$path: expected generated declaration variant not found\n"
      unless @matches == 1;
  qualify_once($content_ref, $path, $matches[0]->[0], $matches[0]->[1]);
}

sub rewrite {
  my ($path, $pairs, $variant_groups) = @_;
  open my $input, '<', $path or die "$path: $!\n";
  local $/;
  my $content = <$input>;
  close $input or die "$path: $!\n";

  for my $pair (@$pairs) {
    qualify_once(\$content, $path, $pair->[0], $pair->[1]);
  }
  for my $variants (@$variant_groups) {
    qualify_variant(\$content, $path, $variants);
  }

  open my $output, '>', $path or die "$path: $!\n";
  print {$output} $content or die "$path: $!\n";
  close $output or die "$path: $!\n";
}

die "usage: $0 FLEX_CC BISON_CC BISON_HH\n" unless @ARGV == 3;
my ($flex_cc, $bison_cc, $bison_hh) = @ARGV;

my @flex_pairs = (
  ['extern FILE *yyin, *yyout;',
   'extern thread_local FILE *yyin, *yyout;'],
  ['extern int yylineno;', 'extern thread_local int yylineno;'],
  ['extern char *yytext;', 'extern thread_local char *yytext;'],
  ['extern int yy_flex_debug;',
   'extern thread_local int yy_flex_debug;'],
  ['static size_t yy_buffer_stack_top',
   'static thread_local size_t yy_buffer_stack_top'],
  ['static size_t yy_buffer_stack_max',
   'static thread_local size_t yy_buffer_stack_max'],
  ['static YY_BUFFER_STATE * yy_buffer_stack',
   'static thread_local YY_BUFFER_STATE * yy_buffer_stack'],
  ['static char yy_hold_char;', 'static thread_local char yy_hold_char;'],
  ['static char *yy_c_buf_p', 'static thread_local char *yy_c_buf_p'],
  ['static int yy_init = 0;', 'static thread_local int yy_init = 0;'],
  ['static int yy_start = 0;', 'static thread_local int yy_start = 0;'],
  ['static int yy_did_buffer_switch_on_eof;',
   'static thread_local int yy_did_buffer_switch_on_eof;'],
  ['FILE *yyin = NULL, *yyout = NULL;',
   'thread_local FILE *yyin = NULL, *yyout = NULL;'],
  ['int yylineno = 1;', 'thread_local int yylineno = 1;'],
  ['int yy_flex_debug = 0;', 'thread_local int yy_flex_debug = 0;'],
  ['static yy_state_type *yy_state_buf=0, *yy_state_ptr=0;',
   'static thread_local yy_state_type *yy_state_buf=0, *yy_state_ptr=0;'],
  ['static char *yy_full_match;',
   'static thread_local char *yy_full_match;'],
  ['static int yy_lp;', 'static thread_local int yy_lp;'],
  ['static int yy_looking_for_trail_begin = 0;',
   'static thread_local int yy_looking_for_trail_begin = 0;'],
  ['static int yy_full_lp;', 'static thread_local int yy_full_lp;'],
  ['static int *yy_full_state;', 'static thread_local int *yy_full_state;'],
  ["\nchar *yytext;", "\nthread_local char *yytext;"],
  ['static int yy_start_stack_ptr = 0;',
   'static thread_local int yy_start_stack_ptr = 0;'],
  ['static int yy_start_stack_depth = 0;',
   'static thread_local int yy_start_stack_depth = 0;'],
  ['static int *yy_start_stack = NULL;',
   'static thread_local int *yy_start_stack = NULL;'],
);

my @flex_variant_groups = (
  [
    ['extern int yyleng;', 'extern thread_local int yyleng;'],
    ['extern yy_size_t yyleng;',
     'extern thread_local yy_size_t yyleng;'],
  ],
  [
    ['static int yy_n_chars;', 'static thread_local int yy_n_chars;'],
    ['static yy_size_t yy_n_chars;',
     'static thread_local yy_size_t yy_n_chars;'],
  ],
  [
    ["\nint yyleng;", "\nthread_local int yyleng;"],
    ["\nyy_size_t yyleng;", "\nthread_local yy_size_t yyleng;"],
  ],
);

rewrite($flex_cc, \@flex_pairs, \@flex_variant_groups);

# The Bison parser is pure: lookahead, semantic/location values, and its error
# counter are automatic variables. Only the legacy Flex scanner still needs the
# checked thread-local transformation above.
