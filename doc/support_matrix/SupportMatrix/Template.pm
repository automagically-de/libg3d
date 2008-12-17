#!/usr/bin/env perl

package SupportMatrix::Template;
use strict;

my $import_dir = "../../plugins/import";
my $tmpl_dir = "../../doc/support_matrix";

sub new {
	my $class = shift;
	my $self = {};

	$self->{template} = {};
	$self->{template_order} = [];

	open TMPL, "<$tmpl_dir/type.tmpl" or die "could not open template";

	while(readline TMPL) {
		next if /^(\s*$|#)/;
		if(/^--/) {
			push @{$self->{template_order}}, "--";
			next;
		}
		split /:\s*/, $_, 2;
		${$self->{template}}{$_[0]} = $_[1];
		chomp ${$self->{template}}{$_[0]};
		push @{$self->{template_order}}, $_[0];
	}
	close TMPL;

	bless $self, $class;
	return $self;
}
