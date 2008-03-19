#!/usr/local/bin/perl
require "./config.inc.cgi";
# get the language
$tmplang=$ENV{'QUERY_STRING'};
$tmplang=~/lang=[A-Za-z]{2}/i;
$langstr=$&;
$langstr='lang=' if($langstr eq '');
($lang,$lang)=split(/=/,$langstr);

# ライブラリ取込
require './joyful-str.cht.pl';
require './joyful-str.jpn.pl' if($lang eq 'ja');
require './cgi-lib.pl';
&ReadParse;

if($in{'mode'} eq '') {
	&top;
} else {
	if ($in{'password'} eq $pass) {
		if ($in{'mode'} eq 'cap') {#キャップ発行
			if(!$in{'action'}) {
				&cap_form;
			} else {
				&cap_publish;
			}
		} elsif ($in{'mode'} eq 'dlcap') {#キャップ抹消
			if(!$in{'action'}) {
				&dlcap_form;
			} else {
				&dlcap;
			}
		} elsif ($in{'mode'} eq 'allcap') {#キャップ一覧
			&cap_view;
		} else {
			&error($jnstr_wrongreq);#処理モードが不明
		}
	}
}


#パスワード暗号処理
sub encrypt {
	local($inpw) = $_[0];
	local(@SALT, $salt, $encrypt);

	@SALT = ('a'..'z', 'A'..'Z', '0'..'9', '.', '/');
	srand;
	$salt = $SALT[int(rand(@SALT))] . $SALT[int(rand(@SALT))];
	$encrypt = crypt($inpw, $salt) || crypt ($inpw, '$1$' . $salt);
	return $encrypt;
}

#パスワード照合処理
sub decrypt {
	local($inpw, $logpw) = @_;
	local($salt, $check);

	$salt = $logpw =~ /^\$1\$(.*)\$/ && $1 || substr($logpw, 0, 2);
	$check = "no";
	if (crypt($inpw, $salt) eq $logpw || crypt($inpw, '$1$' . $salt) eq $logpw)
		{ $check = "yes"; }
	return $check;
}
#キャップ一覧
sub cap_view {
	my(@caplist,@capdata);
	open(FH,"$capfile")||&error("Open Error : $capfile");
	@caplist = <FH>;
	close(FH);
	print <<"EOF";
Content-type: text/html

<html>
<head>
<title>$jnstr_modcap_showall</title>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body bgcolor="#EFEFEF">
<font size="4" color="red">$jnstr_modcap_showall</font>
<br>
<HR size="1">
<form action="mod_cap.cgi" method="post">
<input type="hidden" name="mode" value="dlcap">
<input type="hidden" name="action" value="1">
<input type="hidden" name="password" value="$in{'password'}">
$jnstr_modcap_stop_hint<br>

EOF
	#一覧表示
	foreach (@caplist) {
		@capdata = split(/<>/,$_);
		chomp($capdata[3]);
		print <<"EOF";
●<input type="radio" name="pass" value="$capdata[1]"> <b><font color="$capdata[2]">$capdata[0] ★$capdata[3]</font></b><BR>
EOF
	}
	print <<"EOF";
<input type="submit" value="$jnstr_modcap_stop">
<HR size="1">
<font size="2">(C)http://www.toshinari.net/</font>
</form>
</body>
</html>

EOF

}

#キャップ抹消
sub dlcap {
	my($test,@caplist,@capdata);
	$test = 0;
	open(FH,"$capfile")||&error("Open Error : $capfile");
	@caplist = <FH>;
	chop(@caplist);
	close(FH);
	open(FH,">$capfile");
	foreach (@caplist) {
		@capdata = split(/<>/,$_);
		if($capdata[0] eq $in{'name'}) {
			$test = 1;
		} elsif($capdata[1] eq $in{'pass'}) {
			$test = 1;
		} else {
			print FH $_,"\n";
		}
	}
	close(FH);
	if(!$test) {
		&error("$jnstr_modcap_nocap");
	}
	print <<"EOF";
Content-type: text/html

<html>
<head>
<title>$jnstr_modcap_ereased_title</title>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body bgcolor="#EFEFEF">
<font size="4" color="red">$jnstr_modcap_ereased_header</font>
<br>
$jnstr_modcap_ereased_body
	
<HR size="1">
[<a href="javascript:history.back()">$jnstr_back</a>]
</body>
</html>


EOF


}

#キャップ抹消フォーム
sub dlcap_form {
	print <<"EOF";
Content-type: text/html

<html>
<head>
<title>$jnstr_modcap_erease_title</title>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body bgcolor="#EFEFEF">
<font size="4" color="red">$jnstr_modcap_erease_title</font>
<br>
$jnstr_modcap_erease_hint<BR>
<form action="mod_cap.cgi" method="post">
<input type="hidden" name="mode" value="dlcap">
<input type="hidden" name="action" value="1">
<input type="hidden" name="password" value="$in{'password'}">
<input type="text" name="name" size="10" value=""><input type="submit" value="$jnstr_modcap_stop">
<HR size="1">
(C)http://www.toshinari.net/
</form>
</body>
</html>

EOF

}

#キャップ抹消処理
#キャップ発行
sub cap_publish {
	my(@list,@cap);
	if($in{'name'} =~ /\n|\r|<|>/) {
		&error("$jnstr_modcap_publish_ngchar");
	}
	if(index($in{'name'},"★") > -1) {
		&error("$jnstr_modcap_publish_starchar");
	}
	if(length($in{'name'}) > 48) {
		&error("$jnstr_modcap_publish_longname");
	}
	if(length($in{'pass'}) > 20) {
		&error("$jnstr_modcap_publish_longpass");
	}
	open(FH,"$capfile");
	@list = <FH>;
	close(FH);
	foreach (@list) {
		@cap = split(/<>/,$_);
		if($cap[3] eq $in{'suffix'}) {
			&error("$jnstr_modcap_publish_samecap");
		}
	}
	$in{'pass'}=encrypt($in{'pass'});
	open(FH,">>$capfile");
	print FH "$in{'name'}<>$in{'pass'}<>$in{'color'}<>$in{'suffix'}<>\n";
	close(FH);
	print <<"EOF";
Content-type: text/html\n\n

<html>
<head>
<title>$jnstr_modcap_publish_finish_title</title>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body bgcolor="#EFEFEF">
<font color="red" size="4">$jnstr_modcap_publish_finish_header</font>
<p></p>
●$jnstr_modcap_publish_finish_body
<BR>
●$jnstr_name$jnstr_colon$in{'name'}
<BR>
●$jnstr_modcap_namecolor$jnstr_colon$in{'color'}
<BR>
●$jnstr_modcap_capsuffix$jnstr_colon$in{'suffix'}
<BR>
<HR size="1">
[<a href="javascript:history.back()">$jnstr_back</a>]
<div align=right>(C)http://www.toshinari.net/</div>
</body>
</html>

EOF

}
#キャップの発行フォーム
sub cap_form {
	print <<"EOF";
Content-type: text/html

<html>
<head>
<title>$jnstr_modcap_publish_form</title>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body bgcolor="EFEFEF">
<b>$jnstr_modcap_publish_form</b>
<p>
<form action="mod_cap.cgi" method="post">
●$jnstr_name$jnstr_colon<input type="text" name="name" size="20"><BR>$jnstr_modcap_publish_hint
<BR>
●$jnstr_modcap_publish_cappass$jnstr_colon<input type="text" name="pass" size="10" value="">
<BR>
●$jnstr_modcap_namecolor$jnstr_colon<input type="text" name="color" size="10" value="">
<BR>
●$jnstr_modcap_capsuffix$jnstr_colon<input type="text" name="suffix" size="10" value="">
<BR>
<input type="hidden" name="mode" value="cap">
<input type="hidden" name="action" value="1">
<input type="hidden" name="password" value="$in{'password'}">
<input type="submit" value="$jnstr_submit">
</form><HR>
[<a href="javascript:history.back()">$jnstr_back</a>]
</body>
</html>

EOF

}

#トップ画面
sub top {
	print "Content-type: text/html", "\n\n";
	print <<"EOF";
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<title>$jnstr_admin_cap</title>
</head>
<body>
<CENTER><font size="4" color="red">$jnstr_admin_cap</font></CENTER>
<br>
<form name="form" method="post" action="mod_cap.cgi">
<BR>
$jnstr_proc
<select name="mode">
	<option value="cap">$jnstr_modcap_publish</option>
	<option value="dlcap">$jnstr_modcap_del</option>
	<option value="allcap">$jnstr_modcap_showall</option>
</select>
<br>
<br>
$jnstr_delkey <input type="password" name="password" size="10">
<input type="submit" value=" $jnstr_verify "></p>
</form>
</body>
</html>
EOF
}

#エラーメッセージ
sub error{
	print "Content-type: text/html", "\n\n";
	
	print <<"EOF";
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
</head>
<body bgcolor="#EFEFEF">
<b><font color="red">ERROR</font></b> $_[0]
<BR><BR>
[<a href="javascript:history.back()">$jnstr_back</a>]
<HR>

</body>
</html>

EOF
	exit;
}

