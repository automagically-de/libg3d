#!/usr/bin/env perl

use strict;
use SupportMatrix::Plugins;
use SupportMatrix::Template;
use SupportMatrix::Info;

open HTML, ">matrix.html" or die "could not write to HTML file";

print HTML <<EOD;
<html>
	<head>
		<title>libg3d support matrix</title>
		<style>
		<!--
table { border: 1px solid black; width: 100%; border-collapse: collapse; }
td    { border: 1px solid gray; margin: 2px; }
th    { text-align: right; background-color: #BBB; font-weight: normal; }
		//-->
		</style>
	</head>
	<body>
		<table>
			<tr><td>&nbsp;</td>
EOD

my $plugins = SupportMatrix::Plugins->new();
my $template = SupportMatrix::Template->new();
my %infos = ();

for my $d (@{$plugins->{DIRS}}) {
	$infos{$d} = SupportMatrix::Info->new($d);

	# header
	my $info = \%{$infos{$d}->{INFO}};
	my $types = $info->{"filetypes"};
	$types =~ s#,[:space:]*#<br />#g;
	print HTML "<td>$types</td>\n";
}

print HTML "</tr>\n";

my @colors = (
	"#F04",
	"#D24",
	"#B44",
	"#884",
	"#4B4",
	"#0F4",
	);

for my $o (@{$template->{template_order}}) {
	next if $o =~ /filetypes/;
	print HTML "<tr><th id=\"th-$_\">".${$template->{template}}{$o}."</th>\n";
	for my $d (@{$plugins->{DIRS}}) {
		my $style = undef;
		my $info = \%{$infos{$d}->{INFO}};
		my $text = $info->{$o};
		if($text =~ /^\[([0-5])\]/) {
			$style .= "background-color: ".$colors[$1].";";
			$text =~ s/^\[[0-5]\]//;
		}
		$text =~ s/^$/&nbsp;/;
		print HTML "<td style=\"$style\">$text</td>\n";
	}
	print HTML "</tr>\n";
}

print HTML <<EOD;
</table>
	</body>
</html>
EOD

close HTML;
