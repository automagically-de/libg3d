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
body { background-color: #FADC46; }
table { border: 1px solid black; width: 100%; border-collapse: collapse; }
tr.topline { border-top: 2px solid black; }
tr.default { }
td    { border: 1px solid gray; margin: 2px; background-color: white;
		font-size: small; vertical-align: top; }
span.content { }
th    { text-align: right; background-color: #BBB; font-weight: normal;
		font-size: small; vertical-align: top; }
		//-->
		</style>
		<script>
		<!--
var hidden=false;

function hide_content() {
	var spans = document.getElementsByTagName("span");
	for(i = 0; i < spans.length; i ++) {
		spans[i].style.display = "none";
	}
	hidden = true;
}

function show_content() {
	var spans = document.getElementsByTagName("span");
	for(i = 0; i < spans.length; i ++) {
		spans[i].style.display = "block";
	}
	hidden = false;
}
		//-->
		</script>
	</head>
	<body>
		<a href="javascript:hide_content();">hide content</a> |
		<a href="javascript:show_content();">show content</a>
		<table>
			<tr><td>&nbsp;</td>
EOD

my $plugins = SupportMatrix::Plugins->new();
my $template = SupportMatrix::Template->new();
my %infos = ();
my $class = "topline";

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
	if($o =~ /^--$/) {
		$class="topline";
		next;
	}
	print HTML "<tr class=\"$class\"><th id=\"th-$_\">".
		${$template->{template}}{$o}."</th>\n";
	for my $d (@{$plugins->{DIRS}}) {
		my $style = undef;
		my $info = \%{$infos{$d}->{INFO}};
		my $text = $info->{$o};
		if($text =~ /^\[([0-5])\]/) {
			$style .= "background-color: ".$colors[$1].";";
			$text =~ s/^\[[0-5]\]//;
		}
		$text =~ s/^$/&nbsp;/;
		print HTML "<td style=\"$style\">";
		print HTML "<span class=\"content\">$text</span></td>\n";
		$class="default";
	}
	print HTML "</tr>\n";
}

print HTML <<EOD;
</table>
	</body>
</html>
EOD

close HTML;
