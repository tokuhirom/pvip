use strict;
use warnings;
use utf8;
use Test::More;
use Test::Base::Less;
use File::Temp;

plan tests => 1*blocks();

filters {
    expected => [sub { $_[0] =~ s/\n+\z//; $_[0] }],
};

for my $block (blocks()) {
    my $sexp = parse_re($block->input);
    $sexp =~ s/\n+\z//;
    is $sexp, $block->expected, $block->input;
}

sub parse_re {
    my ($src) = @_;

    my $tmp = File::Temp->new();
    print {$tmp} $src;

    my $sexp = `./qre $tmp`;
    unless ($sexp =~ /\A\(/) {
        die "Cannot get sexp from '$src': $sexp";
    }
    $sexp;
}

__END__

===
--- input: hoge
--- expected
(string "hoge")

===
--- input: [h oge]
--- expected
(elems (string "h")(string "oge"))

===
--- input: [h||oge]
--- expected
(or (string "h")(string "oge"))

===
--- input: [h||oge]
--- expected
(or (string "h")(string "oge"))

