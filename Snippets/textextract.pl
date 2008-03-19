#!/usr/bin/perl

die("Usage: $0 {pattern} {file...} ") if $#ARGV < 1;
$pattern=$ARGV[0];

%matches = ();

while($ARGV[1]) {
	shift @ARGV;
	$file=$ARGV[0];
	if($file eq '-') {
		$fh=STDIN;
	} else {
		open(FIL,$file) || print("Error: $!");
		$fh=FIL;
	}
	while($line = <$fh>) {
		if ($line =~ /$pattern/) {
			$matches{$&} = 1;
			$matches{$&} = 1 while($' =~ /$pattern/);
		}
	}
}

print "$_\n" for(sort keys %matches);