#!/usr/bin/perl

open(DAT, $ARGV[0]) || die("Could not open file ".$ARGV[0]);
while ($file = <DAT>) {
	$file =~ s/\s+$//;
	unlink($file);
}