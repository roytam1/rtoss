#!/usr/bin/perl -w
 
use strict;
use File::Find;
use Digest::MD5;
 
my %files;
my $totalpossibledupfiles;
my $possibledupfilescount;
 
print STDERR "Searching files...\n";
find(\&check_file, $ARGV[0] || ".");
 
local $" = ", ";

$possibledupfilescount = 0;
$totalpossibledupfiles = 0;
foreach my $size (keys %files) {
  next unless @{$files{$size}} > 1;
  $totalpossibledupfiles += $#{$files{$size}}+1;
}

foreach my $size (sort {$b <=> $a} keys %files) {
  next unless @{$files{$size}} > 1;
  my %md5;
  my $scnt = 1;
  foreach my $file (@{$files{$size}}) {
  	++$possibledupfilescount;
    print STDERR "Hashing $file ($size ",$scnt++,"/",$#{$files{$size}}+1,") [",$possibledupfilescount,"/",$totalpossibledupfiles," ",(sprintf "%.2f", $possibledupfilescount*100/$totalpossibledupfiles),"%]...\n";
    open(FILE, $file) or next;
    binmode(FILE);
    push @{$md5{Digest::MD5->new->addfile(*FILE)->hexdigest}},$file;
  }
  foreach my $hash (keys %md5) {
    next unless @{$md5{$hash}} > 1;
    if($size) {
      foreach my $file (@{$md5{$hash}}) {
        print substr($hash,0,8),"\t$size\t$file\n";
      }
    }
  }
}
 
sub check_file {
  -f && push @{$files{(stat(_))[7]}}, $File::Find::name;
}

