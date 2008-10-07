#!/usr/bin/env perl

package SupportMatrix::Info;
use strict;

sub new {
	my $class = shift;
	my ($dir) = @_;
	my $self = {};

	$self->{DIR} = $dir;
	$self->{INFO} = {};

	my $infofile = "$dir/support.info";
	if(-f $infofile) {
		open INFO, "<$infofile" or warn "could not open $infofile";
		while(readline INFO) {
			next if /^(\s*$|#|--)/;
			split /:\s*/, $_, 2;
			${$self->{INFO}}{$_[0]} = $_[1];
			chomp ${$self->{INFO}}{$_[0]};
		}
		close INFO;
	}

	bless $self, $class;
	return $self;
}

1;
