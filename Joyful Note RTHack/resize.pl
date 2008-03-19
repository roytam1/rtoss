package imgbbs;

;# 19:52 2004/08/06
;# resize.pl.re
;# suga@snpn.net

sub note{
	my $note = 'resize.pl.re(2004/08/06)';
	return $note;
}

sub imgresize{
	my ($src,$dst,$w,$h,$jpegq,$del_flag) = @_;

	unless($w =~ /^\d+$/ || $h =~ /^\d+$/){ return 0;}
	my @src = imgbbs::GetImageSize($src);
	unless($src[2] >= 1 && $src[2] <= 3){ return 0; }

	unless($jpegq =~ /^\d+$/){ $jpegq = 75;}
	if($jpegq < 1 || $jpegq > 100){ $jpegq = 75; }

	my $execpg = findfile("repng2jpeg")||findfile("repng2jpeg.exe",1);

	eval{ `$execpg $src $dst $w $h $jpegq`; };

	chmod(0666,$dst);

	if($del_flag == 1){
		unlink($dst) if(-e $dst && (-s $dst) > (-s $src));
	}

	if(-e $dst){ return 1 ;}
	return 0;
}

sub GetImageSize{
	my $src  = $_[0];
	my $buff;
	my $width = 0;	my $height = 0;
	my $formatno = 0;
	
	open(IMG,$src)|| return 0;
	binmode(IMG);
	read(IMG, $buff, 4);
	
	if($buff =~ /^GIF/){
		seek(IMG, 6, 0);
		read(IMG, my $buff, 4);
		$width = unpack("v",substr($buff,0,2));
		$height = unpack("v",substr($buff,2,2));
		$formatno = 1;
	}elsif($buff =~ /PNG/){
		seek(IMG, 16, 0);
		read(IMG, my $buff, 8);
		$width = unpack("N", substr($buff, 0, 4));
		$height = unpack("N", substr($buff, 4, 8));
		$formatno = 3;
	}elsif($buff =~ /^\xff\xd8/){
		seek(IMG, 2, 0);
		while (1) {
			read(IMG, my $buff, 4);
			my ($a, $b, $c) = unpack("a a n", $buff);
			if ($a ne "\xFF") {
				$width = $height = 0; last;
			}elsif ((ord($b) >= 0xC0) && (ord($b) <= 0xC3)){
				read(IMG, $buff, 5);
				($height, $width) = unpack("xnn", $buff);
				last;
			}else{
				read(IMG, my $buff, ($c - 2));
			}
		}
		$formatno = 2;
	}
	close(IMG);
	if($formatno < 1 || $width < 1 || $height < 1){ return 0; }
	return ($width,$height,$formatno,"width=\"$width\" height=\"$height\"");
}

sub findfile{
	my $filename = $_[0];
	if(-e "./$filename"){ 
		if($_[1] ==1) { return ".\\$filename"; }
		else { return "./$filename";}
	}
	my $findpath = $ENV{'PATH'}
		. ':/bin'
		. ':/sbin'
                . ':/usr/bin'
                . ':/usr/ucb'
                . ':/etc'
                . ':/lib'
                . ':/usr/etc'
                . ':/usr/lib'
                . ':/usr/local/bin'
                . ':/usr/local/X11R6/bin'
                . ':/usr/local/bin/mh'
                . ':/usr/local/lib'
                . ':/usr/local/lib/mh'
                . ':/usr/local/sbin'
                . ':/usr/local/libexec'
                . ':/usr/local/canna/bin'
                . ':'.$ENV{'LD_LIBRARY_PATH'};
	my @path = split(/:/,$findpath);
	if($_[1] == 1){ @path = split(/;/,$ENV{'PATH'});}
	foreach my $value (@path){
		if(-e "$value/$filename"){
			if($_[1] == 1){ 
				if($value !~ /\\$/){ $value .= "\\" ;}
				return "$value$filename";
			}
			return "$value/$filename";
		}
	}
	return 0;
}

1;
