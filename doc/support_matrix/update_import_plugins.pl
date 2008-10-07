#!/usr/bin/env perl

use strict;
use SupportMatrix::Template;
use SupportMatrix::Plugins;

my $plugins = SupportMatrix::Plugins->new();
my $template = SupportMatrix::Template->new();

for my $dirname (@{$plugins->{DIRS}}) {
	#print $dirname. "\n";
	my $infofile = "$dirname/support.info";
	my %info = ();
	if(-f $infofile) {
		open INFO, "<$infofile" or die "could not open $infofile";
		while(readline INFO) {
			next if /^(\s*$|#|--)/;
			split /:\s*/, $_, 2;
			$info{$_[0]} = $_[1];
			chomp $info{$_[0]};
		}
		close INFO;
	}

	# don't use hash keys as it may shuffle the items
	open INFO, ">$infofile" or die "could not write to $infofile";
	foreach(@{$template->{template_order}}) {
		my $i = ${$template->{template}}{$_};
		print INFO "# $i\n";
		print INFO $_.": ";
		print INFO $info{$_} if defined $info{$_};
		print INFO "\n";
	}
	# some additional entries in info?
	foreach(keys %info) {
		my $var = $_;
		next if grep $_ eq $var, @{$template->{template_order}};
		print INFO "# unknown entry\n";
		print INFO $var.": ".$info{$var}."\n";
	}
	close INFO;
}

