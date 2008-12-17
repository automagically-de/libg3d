#!/usr/bin/env perl

package SupportMatrix::Plugins;
use strict;

my $import_dir = "../../plugins/import";

sub new {
	my $class = shift;
	my $self = {};
	my @dirs;

	$self->{PLUGIN_DIR} = $import_dir;
	$self->{DIRS} = [];

	opendir DIR, $import_dir or die "could not open import plugin directory";
	while($_ = readdir DIR) {
		next unless /^imp_/;
		my $dirname = "$import_dir/$_";
		next unless -d $dirname;
		push @dirs, $dirname;
	}
	closedir DIR;

	foreach (sort @dirs) {
		push @{$self->{DIRS}}, $_;
	}

	bless $self, $class;
	return $self;
}

1;
