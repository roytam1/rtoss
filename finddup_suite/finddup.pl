#!/usr/bin/perl -w
 
use strict;
use File::Find;
use Digest::MD5;

my $printSize = 1;


my %files;
my $totalpossibledupfiles;
my $totalpossibledupfilessize;
my $possibledupfilescount;
my $possibledupfilescountsize;

($#ARGV < 0) && push @ARGV, ".";
print STDERR "Searching files...\n";
find(\&check_file, @ARGV);
 
local $" = ", ";

$possibledupfilescount = 0;
$totalpossibledupfiles = 0;
$totalpossibledupfilessize = 0;
$possibledupfilescountsize = 0;
foreach my $size (keys %files) {
  next unless @{$files{$size}} > 1;
  $totalpossibledupfiles += $#{$files{$size}}+1;
  $totalpossibledupfilessize += $size*($#{$files{$size}}+1);
}

foreach my $size (sort {$b <=> $a} keys %files) {
  next unless @{$files{$size}} > 1;
  my %md5;
  my $scnt = 1;
  foreach my $file (@{$files{$size}}) {
    ++$possibledupfilescount;
    $possibledupfilescountsize+=$size;
    if($printSize) {
      print STDERR "[",properSize($possibledupfilescountsize),"/",properSize($totalpossibledupfilessize)," ",(sprintf "%.2f", $possibledupfilescountsize*100/$totalpossibledupfilessize),"%] ";
    } else {
      print STDERR "[$possibledupfilescount/$totalpossibledupfiles ",(sprintf "%.2f", $possibledupfilescount*100/$totalpossibledupfiles),"%] ";
    }
    print STDERR "($size ",$scnt++,"/",$#{$files{$size}}+1,") Hashing $file ...\n";
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

sub properSize {
 my $size = shift;
 my $unit = shift || 3;
 my $dp   = shift || 2;

 my @units = ("","KB","MB","GB","TB","PB","EB");
 my $i = 0;
 my $out = "";

 for($i = 0; $i < $unit && $size > 1024; $i++) {
  $size /= 1024;
 }

 $out = sprintf("%.*f",$i?$dp:0,$size);

 # add thousand separators
 $out = reverse $out;
 $out =~ s/(\d{3})(?=\d)(?!\d*\.)/$1,/g;
 $out = reverse $out;

 $out.$units[$i];
}