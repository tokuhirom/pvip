#!/usr/bin/env perl
use strict;
use warnings;
use utf8;
use 5.010000;
use autodie;

my $in  = $ARGV[0] || "src/pvip.h";
my $out = $ARGV[1] || "src/gen.node.c";

open my $h, '<', $in or die "Cannot open $out: $!";

{
    open my $fh, '>', $out or die "Cannot open $out: $!";
    say $fh qq!/* This file is generated from $0 */!;
    say $fh qq!#include "pvip.h"!;
    say $fh qq!const char* PVIP_node_name(PVIP_node_type_t t) {!;
    say $fh qq!  switch (t) {!;
    while (<$h>) {
        if (/(PVIP_NODE_[A-Z0-9_]+)/) {
            my $k = "$1";
            my $v = "$1";
            $v =~ s/^PVIP_NODE_//;
            printf $fh qq!    case %s: return "%s";\n!, $k, lc($v);
        }
    }
    say $fh qq!  }!;
    say $fh qq!  return "UNKNOWN";!;
    say $fh qq!}!;
}
