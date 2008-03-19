#!/usr/bin/perl

die("Usage: $0 {file} {from} {to} ") if $#ARGV < 2;
($file, $from, $to) = @ARGV;
@output = ();

if($file eq '-') {
	$fh=STDIN;
} else {
	open(FIL,$file) || print STDERR "Error: $!\n";
	$fh=FIL;
}
while($line = <$fh>) {
	$line =~ s/$from/$to/g;
	push @output, $line;
}

if($file eq '-') {
	print "@output";
} else {
	open(FIL,">$file") || print STDERR "Error: $!\n";
	for($i=0;$i<=$#output;$i++) {
		print FIL $output[$i];
	}
}
