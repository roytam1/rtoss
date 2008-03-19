#!/usr/local/bin/perl
# 6:05 2004/10/16
# suga@snpn.net
#use strict;	
#$ENV{'LD_LIBRARY_PATH'} = $ENV{'SCRIPT_FILENAME'};
#$ENV{'LD_LIBRARY_PATH'} =~ s/\/[\d\w\.\+\-]+$//;

my $buff;
my $perl;

my @buff;
{#PerlMagick
	my $msg;
	if (eval 'use Image::Magick; 1'){
		$msg .= "PerlMagick OK\n\n";
	}else{
		$msg .= "PerlMagick NG\n\n";
	}
	$buff .= $msg;
}

if($ENV{'PATH'} !~ /C:\\/i){#NetPBM
	my $msg;
	my $cjpeg = findfile("cjpeg");
	my $djpeg = findfile("djpeg");
	my $pngtopnm = findfile("pngtopnm");
	my $giftopnm = findfile("giftopnm");
	my $pnmscale = findfile("pnmscale");
	
	if($cjpeg && $djpeg && $pngtopnm && $giftopnm && $pnmscale){
		$msg .= "NetPBM + libjpeg-progs OK\n";
	}else{
		$msg .= "NetPBM + libjpeg-progs NG\n";
	}

	if($cjpeg){ $msg .= " cjpeg    -> $cjpeg\n"; }
	else{ $msg .= " cjpeg    -> Not Found\n"; }
	if($djpeg){ $msg .= " djpeg    -> $djpeg\n"; }
	else{ $msg .= " djpeg    -> Not Found\n"; }
	if($pngtopnm){ $msg .= " pngtopnm -> $pngtopnm\n"; }
	else{ $msg .= " pngtopnm -> Not Found\n"; }
	if($giftopnm){ $msg .= " giftopnm -> $giftopnm\n"; }
	else{ $msg .= " giftopnm -> Not Found\n"; }
	if($pnmscale){ $msg .= " pnmscale -> $pnmscale\n\n"; }
	else{ $msg .= " pnmscale -> Not Found\n\n"; }
	$buff .= $msg;
}
{#OS
	my ($msg,$find);
	eval {
		$perl = `perl -v`;
		my $perlpath = findfile("perl");
		if(!$perl){ $perl = `$perlpath -v`;}
	};

	if($perl =~ /86/ && $perl =~ /linux/){
		$find = 1;
		my $libjpeg = findfile("libjpeg.so.62");
		my $libpng = findfile("libpng.so.2");
		my $libz = findfile("libz.so.1");
		my $libm = findfile("libm.so.6");
		my $libc = findfile("libc.so.6");
		my $libc = findfile("libc.so.6");
		my $ldlinux = findfile("ld-linux.so.2");
		if($libjpeg && $libpng && $libz && $libm && $libc && $ldlinux){
			$msg .= "repng2jpeg(i386-linux) dynamic :OK\n";
		}else{
			$msg .= "repng2jpeg(i386-linux) dynamic :NG\n";
		}
		if($libz && $libm && $libc && $ldlinux){
			$msg .= "                       standard:OK\n";
		}else{
			$msg .= "                       standard:NG\n";
		}
		$msg .=     "                       static  :OK\n";
		
		if($libjpeg){ $msg .= " libjpeg  -> $libjpeg\n";}
		else{ $msg .= " libjpeg  -> Not Found\n"; }
		if($libpng){ $msg .= " libpng   -> $libpng\n";}
		else{ $msg .= " libpng   -> Not Found\n";}
		if($libz){ $msg .= " libz     -> $libz\n";}
		else{ $msg .= " libz     -> Not Found\n";}
		if($libm){ $msg .= " libm     -> $libm\n";}
		else{ $msg .= " libm     -> Not Found\n";}
		if($libc){ $msg .= " libc     -> $libc\n";}
		else{ $msg .= " libc     -> Not Found\n";}
		if($ldlinux){ $msg .= " ldlinux  -> $ldlinux\n\n";}
		else{ $msg .= " ldlinux  -> Not Found\n\n";}
	}elsif($perl =~ /86/ && $perl =~ /freebsd/){
		$find = 1;
		my $libjpeg = findfile("libjpeg.so.9");
		my $libpng = findfile("libpng.so.5");
		my $libz = findfile("libz.so.2");
		my $libm = findfile("libm.so.2");
		my $libc = findfile("libc.so.4");

		if($libjpeg && $libpng && $libz && $libm && $libc){
			$msg .= "repng2jpeg(i386-freebsd) dynamic :OK\n";
		}else{
			$msg .= "repng2jpeg(i386-freebsd) dynamic :NG\n";
		}
		if($libz && $libm && $libc){
			$msg .= "                         standard:OK\n";
		}else{
			$msg .= "                         standard:NG\n";
		}
		$msg .=     "                         static  :OK\n";

		if($libjpeg){ $msg .= " libjpeg  -> $libjpeg\n";}
		else{ $msg .= " libjpeg  -> Not Found\n"; }
		if($libpng){ $msg .= " libpng   -> $libpng\n";}
		else{ $msg .= " libpng   -> Not Found\n";}
		if($libz){ $msg .= " libz     -> $libz\n";}
		else{ $msg .= " libz     -> Not Found\n";}
		if($libm){ $msg .= " libm     -> $libm\n";}
		else{ $msg .= " libm     -> Not Found\n";}
		if($libc){ $msg .= " libc     -> $libc\n\n";}
		else{ $msg .= " libc     -> Not Found\n\n";}
	}elsif($perl =~ /86/ && $perl =~ /solaris/){
		$find = 1;
		my $libc = findfile("libc.so.1");
		my $libdl = findfile("libdl.so.1");
		if($libc && $libdl){
			$msg .= "repng2jpeg(i386-solaris)  standard:OK\n";
		}else{
			$msg .= "repng2jpeg(i386-solaris)  standard:NG\n";
		}
		$msg .=     "                          static  :OK\n";
		if($libc){ $msg .= " libc     -> $libc\n";}
		else{ $msg .= " libc     -> Not Found\n"; }
		if($libdl){ $msg .= " libdl    -> $libdl\n\n";}
		else{ $msg .= " libdl    -> Not Found\n\n";}
	}elsif($perl =~ /sun4/ && $perl =~ /solaris/){
		$find = 1;
		my $libc = findfile("libc.so.1");
		my $libdl = findfile("libdl.so.1");
		if($libc && $libdl){
			$msg .= "repng2jpeg(sparc-solaris) standard:OK\n";
		}else{
			$msg .= "repng2jpeg(sparc-solaris) standard:NG\n";
		}
		$msg .=     "                          static  :OK\n";
		if($libc){ $msg .= " libc     -> $libc\n";}
		else{ $msg .= " libc     -> Not Found\n"; }
		if($libdl){ $msg .= " libdl    -> $libdl\n\n";}
		else{ $msg .= " libdl    -> Not Found\n\n";}
	}elsif($perl =~ /MSWin32-x86/){
		$find = 1;
		$msg .= "repng2jpeg(i386_win32)\n";
		my $msvcrt = findfile("msvcrt.dll",1);
		if($msvcrt){ $msg .= " msvcrt.dll    -> $msvcrt\n\n";}
		else{ $msg .= " msvcrt.dll    -> Not Found\n\n"; }
	}

	if($find){
		my $execpg = findfile("repng2jpeg")||findfile("repng2jpeg.exe",1);
		if(-e "$execpg"){
			$msg .= "-" x 40 . "\n";
			$msg .= " $execpg Found\n";
			$msg .= '  SIZE : '.(-s $execpg)."\n";
			if(eval'use Digest::MD5 qw(md5_hex); 1' && open(IN,$execpg)){
					binmode(IN);
					read(IN,my $buff,(-s $execpg));
				$msg .= '  MD5  : '.md5_hex($buff)."\n";
			}elsif(findfile("md5")){
				my $tmpcmd = findfile("md5");
				my $tmpmsg = `$tmpcmd $execpg`;
				$msg .= '  MD5  : '.$tmpmsg;
			}elsif(findfile("md5sum")){
				my $tmpcmd = findfile("md5sum");
				my $tmpmsg = `$tmpcmd $execpg`;
				$msg .= '  MD5  : '.$tmpmsg;
			}
			if(-x $execpg){
				$msg .= " $execpg Permisson:OK\n";
				my $tmp;
				eval{ $tmp = `$execpg --test 2>&1`; };
				$tmp =~ s/[\r|\n]//g;
				if($tmp =~ /TestMode:OK/){
					$msg .= " $execpg Exec:OK\n    ->($tmp)\n"
				}else{
					$msg .= " $execpg Exec:NG\n    ->($tmp)\n";
				}
				if(-x findfile("ldd")){
					eval{
						$msg .= "\n\$ldd $execpg\n"; 
						$msg .= `ldd $execpg 2>&1`;
					};
				}
			}else{
				$msg .= " $execpg Permission:NG\n";
			}
		}else{
			$msg .= " repng2jpeg Not Found\n"
		}
	}
	$buff .= $msg . "-" x 40 . "\n";
}
{
	my $msg;
	if ($perl =~ /This\sis\sperl,\s(.*)\n/){
		$msg .= "Perl :$1\n";
	}else{
		$msg .= "Perl :$]\n";
	}
	$msg .= "HTTPd:$ENV{'SERVER_SOFTWARE'}\n";
	$buff .= $msg;
	my $unamepath = findfile("uname");
	my $uname;
	if($unamepath){
		eval{ $uname = `$unamepath -srm`; };
	}
	if(!$uname){
		eval{ $uname = `ver`; };
	}
	if($uname){
		$uname =~ s/[\r\n]//g;
		$buff .= "OS   :$uname\n";
	}
}

print "Content-type: text/html\n\n";
print '<html><head><META HTTP-EQUIV="Content-type" CONTENT="text/html"></head><body><pre>';
print $buff;
print "</pre></body></html>\n";
exit;

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
