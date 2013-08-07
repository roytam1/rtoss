#!/usr/bin/perl
use CGI;
use vars qw(%set %in);
#use strict;
#use subs qw(exit);
#*exit = $ENV{MOD_PERL} ? \&Apache::exit : sub { CORE::exit };
#%set = %in = undef;

$set{'log_file'} = './log.cgi';		#ベースログファイル名
$set{'log_dir'} = './log/';		#ログ作成ディレクトリ
$set{'log_ext'} = '.cgi';		#ログ拡張子
$set{'log_error'} = './error.cgi';		#エラー時のログ
$set{'max_mode'} = 0;		#総スレッド数でログ数規制=0,総投稿数でログ数規制=1,
$set{'max_tree'} = 15;		#最大スレッド数 $set{'max_mode'}=0の時に有効
$set{'max_log'} = 30;		#最大投稿数 $set{'max_mode'}=1の時に有効
$set{'img_dir'} = './src/';		#画像保存ディレクトリ
$set{'thumb_dir'} = './thumb/';		#サムネイル保存ディレクトリ
$set{'title'} = 'うｐろだ';		#タイトル
$set{'top_title'} = '<div align=center><span style="font-size:16pt">うｐろだ</span></div>';		#トップタイトル
$set{'home'} = '../';		#HOMEのリンク http://～も可能
$set{'max_size'} = 300;		#最大投稿容量(KB)
$set{'max_w'} = 250;		#画像最大幅(ピクセル)  幅又は高さがこれ以上になるとサムネイルを作成します
$set{'max_h'} = 250;		#画像最大高(ピクセル) 
$set{'max_gifanime'} = 100;		#設定容量(KB)以上のアニメーションGIFはサムネイルを作成する 使わない場合は$set{'max_size'}以上に
$set{'disp_gifframe'} = 1;		#off=0,アニメーションGIFのフレーム数を表示=1
$set{'pagelog'} = 5;		#1ページに表示するスレッドの数
$set{'res_ry'} = 5;		#1スレッドに表示する投稿数
$set{'admin_pass'} = '0123';		#管理者パスワード
$set{'base_cgi'} = './imgbbs.cgi';		#このスクリプト名
$set{'base_html'} = './imgboard.html';		#入り口1ページ目のファイル名
$set{'base_pre'} = 'img';		#2ページ目以降のファイル接頭詞
$set{'base_ext'} = '.html';		#2ページ目以降のファイル拡張子
$set{'res_ext'} = '.html';		#レスフォームのファイル拡張子
$set{'res_dir'} = './res/';		#レスディレクトリ
$set{'res_dir2'} = '../';		#レスディレクトリからみたスクリプトディレクトリへの相対パス
$set{'resize'} = './resize.pl';		#サムネイル作成ライブラリ
$set{'use_thumb'} = 1;		#サムネイルを作成する 0=off,1=on
$set{'disp_ip'} = 2;		#0=off,1=HOST表示,2=ID表示
$set{'interval'} = 5;		#投稿間隔秒数
$set{'br_max'} = 10;		#改行コードがこれ以上なら投稿させない
$set{'sort'} = 1;		#ソート機能 off=0,on=1(ユーザー任意),on=2(強制)
$set{'deny_host'} = '';		#投稿禁止IP/HOST ,で区切る ex.(bbtec.net,219.119.66,ac.jp)
$set{'admin_cap'} = '管理人,admin';		#管理人キャップ 後者を入力すると管理人の名前で書き込めます
$set{'text'} = '#000000';		#文字の色
$set{'subject'} = '#ff0000';		#スレッドタイトルの色
$set{'bgcolor'} = '#efefef';		#背景色
$set{'name'} = 'green';			#名前の色
$set{'tablecolor'} = '#F0E0D6';		#レス投稿の背景色
$set{'link'} = '#0000FF';		#link 未訪問時のリンク色
$set{'vlink'} = '#800080';		#alink 選択時のリンク色
$set{'alink'} = '#FF0000';		#vlink 既訪問時のリンク色
$set{'cookie_name'} = 'IMG_BBS';		#cookieの名前
$set{'autolink'} = 1;		#自動リンク貼り 0=off,1=on
$set{'encode'} = 3;		# SHIFT_JIS=1,EUC=2,UTF-8=3
$set{'error_level'} = 1;		#0=off, 投稿失敗時のログを記録=1
$set{'delete_level'} = 0;		#0=透明あぼーん, 削除済を残す=1;
$set{'delete_log_level'} = 0;		#off=0,ユーザーが削除された記事を別ログに残す=1
$set{'log_delete'} = './abon.cgi';		#$set{'delete_log_level'} = 1 の時ログを残すファイル
$set{'mime'} = './mime.cgi';	#画像以外のファイルをアップロードさせる場合のmime設定ファイル
$set{'message'} = '
・大きいサイズの画像は縮小表示されます<br>
';
# 分かる人向け
$set{'id_salt'} = 'de';		#IDのsalt 数回cryptする場合は,で区切る ex.3回の場合は 'de,ji,ko' のように設定
$set{'style_sheet'} ='
<META http-equiv="Content-Style-Type" content="text/css">
<style type="text/css">
<!--
body,th,td{ font-size:11pt; }
-->
</style>
';
$set{'per_upfile'} = 0666;		#アップロードファイルのパーミッション suexec=0604,other=0666
$set{'per_html'} = 0666;		#作成HTMLファイルのパーミッション suexec=0604,other=0666
$set{'per_logfile'} = 0666;		#作成ログファイルのパーミッション suexec=0600,other=0666
$set{'per_dir'} = 0777;		#作成ディレクトリのパーミッション suexec=0701,other=0777
$set{'image_check'} = 1;	#画像が不完全な場合 0=強制的にサムネイルを作成する,1=作成しない,2=投稿させない
$set{'break_icon'} = './icon/broken.png';		#$set{'image_check'}=1 の時に表示する画像(JPEG,PNG,GIF)

$ENV{'LD_LIBRARY_PATH'} = $ENV{'SCRIPT_FILENAME'};
$ENV{'LD_LIBRARY_PATH'} =~ s/\/[\d\w\.\+\-]+$//;

#+++++++++++++++++++++++++
$set{'ver'} = "15:52 2003/06/02";
&formdecode;
&formcheck;
if ($in{'mode'} eq 'regist' || $in{'mode'} eq 'res'){ &regist; }
elsif ($in{'mode'} eq 'admin'){ &admin; }
elsif ($in{'mode'} eq 'delete'){ &delete; }
unless(-e $set{'log_file'}){ &init;}
unless(-e $set{'base_html'}){ &makehtml; &quit; }
&error(99);
exit;
#++++++++++++++++++++++++

#----------------
#　デコード 
#----------------
sub formdecode{
	if($set{'encode'} == 1){ $set{'jcode'} = 'SHIFT_JIS'; }
	elsif($set{'encode'} == 2){	$set{'jcode'} = 'euc-jp'; }
	elsif($set{'encode'} == 3){	$set{'jcode'} = 'UTF-8'; }
	$in{'addr'} = $ENV{'REMOTE_ADDR'};
	$in{'host'} = gethostbyaddr(pack('c4',split(/\./, $in{'addr'})), 2) || $in{'addr'};
	$in{'time'} = time;
	$in{'date'} = conv_date($in{'time'});
	if ($ENV{'CONTENT_LENGTH'} > $set{'max_size'} * 1024){ &error(106);}
	$in{'q'} = new CGI;
	my $q = $in{'q'};
	$in{'upfile'} = $q->param('upfile');
	$in{'tmpfile'} = $q->tmpFileName($in{'upfile'});
	$in{'type'} = $q->uploadInfo($in{'upfile'})->{'Content-Type'} if ($in{'upfile'});
	$in{'name'} = $q->param('name');
	$in{'email'} = $q->param('email');
	$in{'sub'} = $q->param('sub');
	$in{'comment'} = $q->param('comment');
	$in{'pass'} = $q->param('pass');
	$in{'reno'} = $q->param('reno');
	$in{'textonly'} = $q->param('textonly');
	$in{'mode'} = $q->param('mode');
	$in{'delno'} = $q->param('delno');
	$in{'delpass'} = $q->param('delpass');
	$in{'treeno'} = $q->param('treeno');
	$in{'delmode'} = $q->param('delmode');
	$in{'deloption'} = $q->param('deloption');
	$in{'jcode'} = $q->param('jcode');
	$in{'org_name'} = $in{'name'};
	$in{'org_email'} = $in{'email'};
	$in{'org_pass'} = $in{'pass'};
	if($in{'mode'} eq 'regist' || $in{'mode'} eq 'res'){
		if($set{'encode'} == 1){ unless ($in{'jcode'} =~ /^\x8a\xbf\x8e\x9a$/){ &error(114); }}
		elsif($set{'encode'} == 2){ unless ($in{'jcode'} =~ /^\xb4\xc1\xbb\xfa$/){ &error(114); }}
		elsif($set{'encode'} == 3){ unless ($in{'jcode'} =~ /^\xe6\xbc\xa2\xe5\xad\x97$/){ &error(114); }}
	}
}


#----------------
# フォームチェック
#----------------
sub formcheck{

	my @denyhost = split(/,/,$set{'deny_host'});
	foreach my $value (@denyhost){
		if ($in{'addr'} =~ /$value/ || $in{'host'} =~ /$value/){ &error(101);}
	}
	my @form = ($in{'name'},$in{'email'},$in{'comment'},$in{'sub'});
	foreach my $value (@form) {
#		$value =~ s/&/&amp;/g;
		$value =~ s/"/&quot;/g;
		$value =~ s/</&lt;/g;
		$value =~ s/>/&gt;/g;
		$value =~ s/\r\r\n/<br>/g;
		$value =~ s/\r\n/<br>/g;
		$value =~ s/\r/<br>/g;
		$value =~ s/\n/<br>/g;
	}
	($in{'name'},$in{'email'},$in{'comment'},$in{'sub'}) = @form;
	if (length($in{'name'}) > 40){ &error(103);}
	if (length($in{'email'}) > 40){ &error(104);}
	if (length($in{'sub'}) > 40){ &error(112);}
	if (length($in{'comment'}) > 1000){ &error(105);}
	if ($in{'mode'} eq 'regist' || $in{'mode'} eq 'res'){
		if (length($in{'comment'}) == 0){ &error(110);}
		my $br = $in{'comment'} =~ s/<br>/<br>/g;
		if ($set{'br_max'} < $br){ &error(113,$br - $set{'br_max'});}
	}
	if ($set{'sort'} == 1 && $in{'email'} =~ /sage/) { $set{'sort'} = 0;}
}

#----------------
#　投稿処理
#----------------
sub regist{
	&filecheck if (($in{'mode'} eq 'regist' || $in{'mode'} eq 'res') && ($in{'upfile'} || $in{'textonly'} ne 'on'));
	eval { close($in{'upfile'}); };
	eval { unlink($in{'tmpfile'}); };
	&logwrite;
	&reswrite;
	&makehtml;
	&makeres($in{'reno'});
	&quit;
}


#----------------
#　記事削除
#----------------
sub delete{
	my $del_flag = 0;
	my $tree_rf = 0;
	open(IN,$set{'log_file'})||&error(301);
	my @tmp = <IN>;
	close(IN);
	my $del_refresh = 0;
	foreach my $value (@tmp){
	my ($treeno,$sub,$file,$allno,$dummy) = split(/<>/,$value);
		if(!$dummy){ next; }
		my $del_tree_f;
		my @allno = split(/,/,$allno);
		foreach my $value2 (@allno){
			if ($in{'delno'} == $value2){
				$del_flag = 1;
				open(IN,"$set{'log_dir'}$treeno$set{'log_ext'}")|| &error(303,"$set{'log_dir'}$treeno$set{'log_ext'}");
				my $reslog = my @reslog = <IN>;
				close(IN);
				foreach my $value3 (@reslog){
					my ($reno,$no,$name,$email,$comment,$date,$host,$id,$pass,$sub,$time,$ext) = split(/<>/,$value3);
					if ($in{'delno'} == $no){
						my $addr = (gethostbyname($host))[4];
						$addr = sprintf("%u.%u.%u.%u", unpack("C*", $addr));
						
						if ($addr =~ /(\d+).(\d+).(\d+).(\d+)/){ $addr = "$1.$2.$3"; }
						if ($in{'addr'} =~ /^$addr/){
							$del_flag = 2;
						}else{
							if($in{'delpass'} eq ''){ &error(402);}
							my $salt = substr($pass, 0, 2);
							my $delpass = crypt($in{'delpass'},$salt);
							if ($delpass eq $pass){ $del_flag=2; }
						}
						if ($del_flag < 2){	if ($in{'delpass'} eq $set{'admin_pass'}){ $del_flag=3; }}
						
						if($del_flag >= 2 && $set{'delete_log_level'} == 1){
							open(OUT,">>$set{'log_delete'}");
							print OUT $value3;
							close(OUT);
							chmod($set{'per_logfile'},$set{'log_delete'});
						}
						if($del_flag == 2){
							$value3 = "$reno<>$no<>$name<><>この記事は投稿者により削除されました<>削除<>deleted:$host<>$id<><><>$time<><><><><><><>1\n";
						}elsif($del_flag == 3){
							$value3 = "$reno<>$no<>$name<><>この記事は管理者により削除されました<>削除<>deleted:$host<>$id<><><>$time<><><><><><><>1\n";
						}
						if($del_flag >= 2 && $ext){
							unlink("$set{'img_dir'}$time$ext");
							unlink <$set{'thumb_dir'}${time}s.*>;
						}
						if($del_flag >= 2 && $reno != $no && $set{'delete_level'} == 0){ undef $value3; }
					}
				}
				if($del_flag >= 2){
					if ($reslog == 1){
						$del_tree_f = 1;
						unlink ("$set{'log_dir'}$treeno$set{'log_ext'}");
						unlink ("$set{'res_dir'}res$treeno$set{'res_ext'}");
					}else{
						open(OUT,"+>$set{'log_dir'}$treeno$set{'log_ext'}")||&error(304);
						eval{ flock(OUT, 2);};
						eval{ truncate(OUT, 0);};
						seek(OUT, 0, 0);
						print OUT @reslog;
						eval{ flock(OUT, 8);};
						close(OUT);
						&makeres($treeno);
					}
				}
			}
		}
		if($del_tree_f == 1){ undef $value; $del_refresh = 1;}
	}
	if($del_refresh == 1){
			open(OUT,"+>$set{'log_file'}")||&error(302);
			eval{ flock(OUT, 2);};
			eval{ truncate(OUT, 0);};
			seek(OUT, 0, 0);
			print OUT @tmp;
			eval{ flock(OUT, 8);};
			close(OUT);
	}
	if ($del_flag == 0){ &error(401); }
	elsif ($del_flag == 1){ &error(402); }
	else{ &makehtml; &quit; }
}

#----------------
# adminモード
#----------------
sub admin{
	my $buff;
	my $header =<<"EOM";
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang=ja>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=$set{'jcode'}">
<title>$set{'title'}</title>
</head>
<body text="$set{'text'}" bgcolor="$set{'bgcolor'}" link="$set{'link'}" alink="$set{'link'}" vlink="$set{'vlink'}">
<div align=right>[<a href="$set{'base_html'}">掲示板に戻る</a>] [<a href="$set{'home'}" target="_top">ホーム</a>]</div>
EOM

	if(!$in{'pass'}){
	print "Content-type: text/html\n\n";
	print $header;
	print <<"EOM";
<div align=center>管理モード
<form action="$set{'base_cgi'}" method="POST">
<input type=password size=8 name=pass value=""><br>
<input type=hidden name=mode value=admin>
<br>
<input type=submit value=submit>
</form>
</div>
</body></html>
EOM
	exit;
	}elsif($in{'pass'} ne $set{'admin_pass'}){
		&error(501);
	}
	
	if(!$in{'delmode'}){
		$buff =<<"EOM";
<div align=center>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
EOM
		$buff .=<<'EOM';
<select name="delmode">
<option value="tree_view">スレッド一覧表示/削除</option>
<option value="check">CGIチェック</option>
<option value="errorlog_view">システムエラー表示</option>
<option value="refresh">HTMLを全て更新する</option>
<option value="restore">ログ復旧</option>
</select>
<br><br>
<input type="submit" value="submit">
</form></div></body></html>
EOM
	}
	elsif($in{'delmode'} eq 'tree_view'){
		open(IN,$set{'log_file'})||&error(301);
		my @tmp = <IN>;
		close(IN);
		my ($no,$host,$time)=split(/<>/,$tmp[0]);	
		$buff =<<"EOM";
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="管理モードに戻る">
</form>
EOM
		$buff .= "最終投稿<br>No.$no/Host:$host/".localtime($time)."<br><br>\n";
		$buff .= "<table border=1 summary=\"スレッド一覧\"><tr><td></td><td></td><td>スレッドNo</td><td>タイトル</td><td>作成時間</td><td>画像</td><td>レス数</td><td>レスリスト</td></tr>\n";
		shift(@tmp);
		my $img_count = 0;
		my $size_count = 0;
		my $res_count = 0;
		my $tree_count = 0;
		my $buff2;
		foreach my $value (@tmp){
			my ($no,$sub,$file,$allno,$dummy) = split(/<>/,$value);
			my $res_num = split(/,/,$allno);
			my ($time,$ext) = split(/\./,$file);
			if (-e "$set{'img_dir'}$file"){
				$img_count++;
				my $size = (-s "$set{'img_dir'}$file");
				$file = "<a href=\"$set{'img_dir'}$file\" target=\"_blank\">$file (".int($size/1024)."KB)</a>";
				$size_count += $size;
			}elsif($ext){
				$file = "$file(削除)";
			}else{ undef $file; }
			
			$res_count += $res_num;
			$tree_count++;
			my $date = conv_date($time);
			$buff2 .= "<tr><td><form action=\"$set{'base_cgi'}\" method=\"POST\"><input type=\"hidden\" name=\"treeno\" value=\"$no\"><input type=\"hidden\" name=\"delmode\" value=\"deltree\"><input type=\"hidden\" name=\"mode\" value=\"admin\"><input type=hidden name=\"pass\" value=\"$in{'pass'}\"><input type=\"submit\" value=\"削除\"></form></td><td><form action=\"$set{'base_cgi'}\" method=\"POST\"><input type=\"hidden\" name=\"treeno\" value=\"$no\"><input type=hidden name=\"delmode\" value=\"logview\"><input type=hidden name=\"mode\" value=\"admin\"><input type=hidden name=\"pass\" value=\"$in{'pass'}\"><input type=\"submit\" value=\"詳細\"></form></td><td>$no</td><td>$sub</td><td>$date</td><td>$file</td><td>$res_num</td><td>$allno</td></tr>\n";
		}
		if ($size_count > 1048576){
			$size_count = sprintf("%.1fMB",$size_count/1048576);
		}elsif($size_count > 1024){
			$size_count = int($size_count/1024)."KB";
		}else{
			$size_count .= "B";
		}
		$buff .= "<tr><td></td><td></td><td>総数 $tree_count</td><td></td><td></td><td>総数 $img_count/$size_count</td><td>総数 $res_count</td></tr>\n".$buff2."</table>";
	}elsif($in{'delmode'} eq 'logview'){
		open(IN,"$set{'log_dir'}$in{'treeno'}$set{'log_ext'}")|| &error(303,"$set{'log_dir'}$in{'treeno'}$set{'log_ext'}");
		my @log = <IN>;
		close(IN);
		my $i;
		my $dispid = 1;
		my $disphost = 1;
		foreach my $value (@log){
			my ($reno,$no,$name,$email,$comment,$date,$host,$pass,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size) = split(/<>/,$value);
			$i++;
			if ($reno == $no){
				$buff =<<"EOM";
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="hidden" name="delmode" value="tree_view">
<input type="submit" value="スレッド一覧に戻る">
</form>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="treeno" value="$in{'treeno'}">
<input type="hidden" name="delmode" value="deltree">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="このスレッドを削除する">
</form>
EOM
				$buff .= make_item($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,1);
			}else{
				$buff .= make_item($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,2);
			}

		}
	}elsif($in{'delmode'} eq 'deltree'){
		open(IN,$set{'log_file'})||&error(301);
		my @tmp = <IN>;
		close(IN);
		foreach my $value (@tmp){
			my ($treeno,$sub,$file,$allres,$dummy) = split(/<>/,$value);
			if (!$dummy){ next; }
			if($treeno == $in{'treeno'}){
				my ($time,$ext) = split(/\./,$file);
				unlink("$set{'img_dir'}$file");
				unlink <$set{'thumb_dir'}${time}s.*>;
				unlink("$set{'log_dir'}$treeno$set{'log_ext'}");
				unlink("$set{'res_dir'}res$treeno$set{'res_ext'}");
				undef $value;
			}
		}
		
		open(OUT,"+>$set{'log_file'}")||&error(302);
		eval{ flock(OUT, 2);};
		eval{ truncate(OUT, 0);};
		seek(OUT, 0, 0);
		print OUT @tmp;
		eval{ flock(OUT, 8);};
		close(OUT);
		&makehtml;
		
		$buff =<<"EOM";
<div align=center>
完了しました<br>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="hidden" name="delmode" value="tree_view">
<input type="submit" value="スレッド一覧に戻る">
</form>
</div>
</body></html>
EOM
	}elsif($in{'delmode'} eq 'check'){
		$buff =<<"EOM";
<div align=center>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="管理モードに戻る">
</form>
EOM
		if(open(IN,$set{'log_file'})){
			$buff .= "スレッドログファイル $set{'log_file'} 読み込み OK<br>\n";
			close(IN);
			if(open(OUT,">>$set{'log_file'}")){
				$buff .= "スレッドログファイル $set{'log_file'} 書き込み OK<br>\n";
				close(OUT);
			}else{
				$buff .= "スレッドログファイル $set{'log_file'} 書き込み NG<br>\n";
			}
		}else{
			$buff .= "スレッドログファイル $set{'log_file'} 読み込み NG<br>\n";
			$buff .= "スレッドログファイル $set{'log_file'} 書き込み NG<br>\n";
		}
		if(open(IN,$set{'log_error'})){
			$buff .= "エラーログファイル $set{'log_error'} 読み込みOK<br>\n";
			close(IN);
			if(open(OUT,">>$set{'log_error'}")){
				$buff .= "エラーログファイル $set{'log_error'} 書き込み OK<br>\n";
				close(OUT);
			}else{
				$buff .= "エラーログファイル $set{'log_error'} 書き込み NG<br>\n";
			}
		}else{
			$buff .= "エラーログファイル $set{'log_error'} 読み込み NG<br>\n";
			$buff .= "エラーログファイル $set{'log_error'} 書き込み NG またはファイルが存在しません<br>\n";
		}
		if ($set{'delete_log_level'} >= 1){
			if(open(IN,$set{'log_delete'})){
				$buff .= "削除ログファイル $set{'log_delete'} 読み込みOK<br>\n";
				close(IN);
				if(open(OUT,">>$set{'log_delete'}")){
					$buff .= "削除ログファイル $set{'log_delete'} 書き込み OK<br>\n";
					close(OUT);
				}else{
					$buff .= "削除ログファイル $set{'log_delete'} 書き込み NG<br>\n";
				}
			}else{
				$buff .= "削除ログファイル $set{'log_delete'} 読み込み NG<br>\n";
				$buff .= "削除ログファイル $set{'log_delete'} 書き込み NG またはファイルが存在しません<br>\n";
			}
		}
		if(open(OUT,">$set{'log_dir'}$in{'time'}.tmp")){
			$buff .= "ログディレクトリ $set{'log_dir'} 書き込み OK<br>\n";
			close(OUT);
			unlink("$set{'log_dir'}$in{'time'}.tmp");
		}else{
			$buff .= "ログディレクトリ $set{'log_dir'} 書き込み NG<br>\n";
		}
		if(open(OUT,">$set{'res_dir'}$in{'time'}.tmp")){
			$buff .= "ログディレクトリ $set{'res_dir'} 書き込み OK<br>\n";
			close(OUT);
			unlink("$set{'res_dir'}$in{'time'}.tmp");
		}else{
			$buff .= "レスフォームディレクトリ $set{'res_dir'} 書き込み NG<br>\n";
		}
		if(open(OUT,">$set{'img_dir'}$in{'time'}.tmp")){
			$buff .= "画像保存ディレクトリ $set{'img_dir'} 書き込み OK<br>\n";
			close(OUT);
			unlink("$set{'img_dir'}$in{'time'}.tmp");
		}else{
			$buff .= "画像保存ディレクトリ $set{'img_dir'} 書き込み NG<br>\n";
		}
		if(open(OUT,">$set{'thumb_dir'}$in{'time'}.tmp")){
			$buff .= "サムネイル保存ディレクトリ $set{'thumb_dir'} 書き込み OK<br>\n";
			close(OUT);
			unlink("$set{'thumb_dir'}$in{'time'}.tmp");
		}else{
			$buff .= "サムネイル保存ディレクトリ $set{'thumb_dir'} 書き込み NG<br>\n";
		}

		$buff .= "</div>";
	}elsif($in{'delmode'} eq 'errorlog_view'){
		if($in{'deloption'} == 1){
			open(OUT,">$set{'log_error'}");
			close(OUT);
			chmod($set{'per_logfile'},$set{'log_error'});
		}
		$header .=<<"EOM";
<div align=center>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="管理モードに戻る">
</form>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="hidden" name="delmode" value="errorlog_view">
<input type="hidden" name="deloption" value=1>
<input type="submit" value="ログをクリア">
</form>
EOM
		if(open(IN,$set{'log_error'})){
			my @tmp = <IN>;
			close(IN);
			@tmp = reverse(@tmp);
			foreach my $value (@tmp){
				$buff .= $value;
			}
			if($buff eq ''){ $buff = "現在エラーログはありません"; }
			$buff = "<textarea cols=60 rows=15 wrap=\"off\">$buff</textarea>";
		}else{
			$buff = "エラーログ記録ファイルが読み込めませんでした";
		}
		$buff .= "</div>";
	}elsif($in{'delmode'} eq 'refresh'){
		open(IN,$set{'log_file'})||&error(301);
		my @tmp = <IN>;
		close(IN);
		foreach my $value (@tmp){
			my ($treeno,$sub,$file,$allres,$dummy) = split(/<>/,$value);
			if($dummy){
				&makeres($treeno);
			}
		}
		&makehtml;
		$buff =<<EOM;
<div align=center>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="管理モードに戻る">
</form><br>
完了しました
</div>
EOM
	}elsif($in{'delmode'} eq 'restore'){
		if ($in{'deloption'} == 1){
			my @new = ();
			my @dir2 = ();
			my $lastno = 0;
			opendir(DIR, $set{'log_dir'}) ||&error(306);
			my @dir = reverse(sort readdir(DIR));
 			closedir(DIR);
 			foreach my $value (@dir){
 				if ($value =~ /(\d+)$set{'log_ext'}$/){
 					push(@dir2,$1);
 				}
 			}
 			@dir2 = reverse(sort {$a <=> $b} @dir2);
 			foreach my $value (@dir2){
 				my $tmp;
 				open(IN,"$set{'log_dir'}$value$set{'log_ext'}");
 				my @tmp = <IN>;
 				close(IN);
 				foreach my $value2 (@tmp){
 					my ($reno,$no,$name,$email,$comment,$date,$host,$pass,$id,$sub,$time,$ext) = split(/<>/,$value2);
 					if($reno == $no){
 						$tmp = "$reno<>$sub<>$time";
 						if($ext){ $tmp .= "$ext"; }
 						$tmp .= "<>$reno";
 					}else{
 						$tmp .= ",$no";
 					}
 					if ($lastno < $no) {$lastno = $no;}
 				}
 				$tmp .= "<>1\n";
				push(@new,$tmp);
 			}
			open(OUT,"+>$set{'log_file'}")||&error(302);
			eval{ flock(OUT, 2);};
			eval{ truncate(OUT, 0);};
			seek(OUT, 0, 0);
			print OUT "$lastno<>0<>0<>1\n";
			print OUT @new;
			eval{ flock(OUT, 8);};
			close(OUT);
			chmod($set{'per_logfile'},$set{'log_file'});
			
			$buff =<<"EOM";
<br><div align=center>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="管理モードに戻る">
</form><br>
完了しました
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="hidden" name="delmode" value="refresh">
<input type="submit" value="HTMLを全て更新する">
</form></div>
EOM
		}else{
			$buff =<<"EOM";
<br><div align=center>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="submit" value="管理モードに戻る">
</form>
<br>
現在のログディレクトリにある<br>
スレッドログファイルからスレッド一覧ログの復旧を試みます<br>
正常時には使わないようにしてください<br>
<form action="$set{'base_cgi'}" method="POST">
<input type="hidden" name="mode" value="admin">
<input type="hidden" name="pass" value="$in{'pass'}">
<input type="hidden" name="delmode" value="restore">
<input type="hidden" name="deloption" value=1>
<input type="submit" value="実行する">
</form></div>
EOM
		}
	}else{
		&error(502);
	}
	print "Content-type: text/html\n\n";
	print $header.$buff;
	print "</body></html>";
	exit;
}

#----------------
# メインログ書き込み
#----------------
sub logwrite{

	open(IN,$set{'log_file'})||&error(301);
	my @tmp = <IN>;
	close(IN);
	
	my ($no,$host,$time,$dummy) = split(/<>/,$tmp[0]);
	$in{'no'} = $no + 1;
	if ($in{'time'} < $time+ $set{'interval'} && $host eq $in{'host'}){
		if ($in{'ext'}) { unlink("$set{'img_dir'}$in{'time'}$in{'ext'}"); }
		&error(111);
	}
	shift(@tmp);
	my @new = ();
	if(!$in{'reno'}){
		$new[0] = "$in{'no'}<>$in{'host'}<>$in{'time'}<>1\n";
		$new[1] = "$in{'no'}<>$in{'sub'}<>$in{'time'}$in{'ext'}<>$in{'no'}<>1\n";
		push(@new,@tmp);
	}else{
		my ($res_flag,$i);
		foreach my $value (@tmp){
			my ($treeno,$sub,$file,$allres,$dummy) = split(/<>/,$value);
				if ($in{'reno'} == $treeno){
					$value = "$treeno<>$sub<>$file<>$allres,$in{'no'}<>1\n";
					$res_flag = 1;
				}
			if (!$res_flag) {$i++;}
		}
		unless($res_flag == 1){&error(109);}
		if ($set{'sort'} == 0){
			$new[0] = "$in{'no'}<>$in{'host'}<>$in{'time'}<>1\n";
			push(@new,@tmp);
		}else{
			$new[0] = "$in{'no'}<>$in{'host'}<>$in{'time'}<>1\n";
			$new[1] = $tmp[$i];
			splice(@tmp, $i,1);
			push(@new,@tmp);
		}
	}

	if(!$in{'reno'}){
		if ($set{'max_mode'} == 0){
			my $new = @new - 1;
			while($set{'max_tree'} < $new){
				 my ($treeno,$sub,$file,$allres,$dummy) = split(/<>/,$new[$new]);
				 open(IN,"$set{'log_dir'}$treeno$set{'log_ext'}")||error(303,"$set{'log_dir'}$treeno$set{'log_ext'}");
				 my @log = <IN>;
				 close(IN);
				 my ($reno,$no,$name,$email,$comment,$date,$host,$pass,$id,$sub,$time,$ext) = split(/<>/,$log[0]);
				 unlink("$set{'img_dir'}$time$ext");
				 unlink <$set{'thumb_dir'}${time}s.*>;
				 unlink("$set{'log_dir'}$treeno$set{'log_ext'}");
				 unlink("$set{'res_dir'}res$treeno$set{'res_ext'}");
				 pop(@new);
			 $new = @new - 1;
			}
		}else{
			my $i = -1; my $tmp = 0;
			foreach my $value (@new){
				$i++;
				my ($treeno,$sub,$file,$allres,$dummy) = split(/<>/,$value);
				next if(!$dummy);
				my @tmp  = split(/,/,$allres);
				$tmp += @tmp;
				if ($tmp > $set{'max_log'}){
					my ($treeno,$sub,$file,$allres,$dummy) = split(/<>/,$new[$i]);
					
					open(IN,"$set{'log_dir'}$treeno$set{'log_ext'}")||error(303,"$set{'log_dir'}$treeno$set{'log_ext'}");
				 	my @log = <IN>;
					close(IN);
					
					my ($reno,$no,$name,$email,$comment,$date,$host,$pass,$id,$sub,$time,$ext) = split(/<>/,$log[0]);
					unlink("$set{'img_dir'}$time$ext");
					unlink <$set{'thumb_dir'}${time}s.*>;
					unlink("$set{'log_dir'}$treeno$set{'log_ext'}");
					unlink("$set{'res_dir'}res$treeno$set{'res_ext'}");
					undef $value;
				}
			}
		}
	}
	open(OUT,"+>$set{'log_file'}")||&error(302);
	eval{ flock(OUT, 2);};
	eval{ truncate(OUT, 0);};
	seek(OUT, 0, 0);
	print OUT @new;
	eval{ flock(OUT, 8);};
	close(OUT);
}
#----------------
#　レスログ書き込み　ついでにID/トリップ作成
#----------------
sub reswrite{

	$in{'reno'} = $in{'no'} if (!$in{'reno'});
	$in{'name'} = '名無しさん' if (!$in{'name'});
	my @tmp = ('a'..'z', 'A'..'Z', '0'..'9', '.', '/');
	srand;
	my $tmp = $tmp[int(rand(@tmp))] . $tmp[int(rand(@tmp))];
	$in{'pass'} = crypt($in{'pass'}, $tmp);

	$in{'name'} =~ s/★/☆/g;
	$in{'name'} =~ s/◆/◇/g;

	if($in{'name'} =~ /^(.*)\#(.+)$/){
		my $name = $1;	my $key = $2;
#		$key =~ s/\xef\xbd\x9e/\xe3\x80\x9c/g; # for UTF-8
#		require './jcode.pl'; &jcode::euc2sjis(\$key);
#		use Jcode; $key = Jcode->new($key, 'euc')->sjis;
#		use Jcode; $key = Jcode->new($key, 'utf8')->sjis;
#		use Encode; $key = encode('sjis', decode('euc-jp', $key));
#		use Encode; $key = encode('sjis', decode('utf8', $key));
		my $salt = substr($key."H",1,2);
		$salt =~ tr/\x00-\x20\x7B-\xFF/./;
		$salt =~ tr/\x3A-\x40\x5B-\x60/A-Ga-f/;
		my $code = substr(crypt($key,$salt),-10);
		$in{'name'} = $1."</b>◆$code<b>";
	}
	
	if($in{'addr'} =~ /(\d+).(\d+).(\d+).(\d+)/){#ID生成
		my @date = localtime($in{'time'});
		my $tmp = "$4$3$2";
		$tmp = crypt($tmp,$date[3]);
		$tmp = crypt($tmp,$1);
		$tmp = substr($tmp, -9, 8);
		my @tmp = split(/,/,$set{'id_salt'});
		foreach my $value (@tmp){
			$tmp = crypt($tmp,$value);
			$tmp = substr($tmp, -9, 8);
		}
		$in{'id'} = $tmp;
	}
	
	my ($adminname,$adminpass) = split(/,/,$set{'admin_cap'});
	if ($adminname && $adminpass){
		$in{'name'} =~ s/$adminname/"$adminname"/i;
		$in{'name'} =~ s/$adminpass/$adminname ★/i;
	}

	open(OUT,">>$set{'log_dir'}$in{'reno'}$set{'log_ext'}") || &error(304,"$set{'log_dir'}$in{'reno'}$set{'log_ext'}");
	print OUT "$in{'reno'}<>$in{'no'}<>$in{'name'}<>$in{'email'}<>$in{'comment'}<>$in{'date'}<>$in{'host'}<>$in{'pass'}<>$in{'id'}<>$in{'sub'}<>$in{'time'}<>$in{'ext'}<>$in{'W'}<>$in{'H'}<>$in{'newW'}<>$in{'newH'}<>$in{'size'}<>1\n";
	close(OUT);
	chmod($set{'per_logfile'},"$set{'log_dir'}$in{'reno'}$set{'log_ext'}");
}

#----------------
# 基本HTML作成
#----------------
sub makehtml{
	open(IN,$set{'log_file'})||&error(301);
	my @tmp = <IN>;
	close(IN);
	shift(@tmp);
	my $tmp = @tmp;
	my $lastpage = int(($tmp - 1) / $set{'pagelog'}) + 1;
	my $header =<<"EOM";
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang=ja>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=$set{'jcode'}">
<META HTTP-EQUIV="CONTENT-SCRIPT-TYPE" CONTENT="text/javascript">
<script type="text/javascript">
<!--
var cookiename = "$set{'cookie_name'}";function getCookie(obj)
{var i, str;c = new Array(); p = new Array("", "", "");
str = document.cookie;c = str.split(";");
for (i = 0; i < c.length; i++) {if (c[i].indexOf(cookiename+"=") >= 0) {
p = (c[i].substr(c[i].indexOf("=")+1)).split("<>");break;}}
obj.name.value =  unescape(p[0]); obj.email.value = unescape(p[1]);
obj.pass.value =  unescape(p[2]);
return true;}
//-->
</script>
<title>$set{'title'}</title>
$set{'style_sheet'}
</head><body text="$set{'text'}" bgcolor="$set{'bgcolor'}" link="$set{'link'}" alink="$set{'link'}" vlink="$set{'vlink'}" onload="getCookie(document.Form);"><blockquote>
<div align=right>[<a href="$set{'home'}" target="_top">ホーム</a>] [<a href="$set{'base_cgi'}?mode=admin">管理用</a>]</div>
$set{'top_title'}<br><hr size=1>
<form action="$set{'base_cgi'}" method="POST" enctype="multipart/form-data" name="Form">
<table border=0 cellspacing=0 summary="投稿フォーム">
<tr>
<td nowrap><input type=hidden name=mode value="regist">
<input type="hidden" name="jcode" value="漢字">おなまえ</td>
<td><input type=text name=name size=35 value=""></td>
</tr><tr>
<td nowrap>email</td>
<td><input type=text name=email size=35 value=""></td>
</tr><tr>
<td nowrap>題名：</td>
<td nowrap><input type=text name=sub size=35 value=""></td>
</tr><tr>
<td nowrap>本文</td>
<td><textarea name=comment COLS=40 ROWS=3></textarea></td></tr>
<tr><td>画像選択</td><td><input type="file" name="upfile" size="30"> <input type="checkbox" name="textonly" value="on">画像なし</td></tr>
<tr><td nowrap>削除キー</td><td><input type="password" size=8 name="pass" value=""></td></tr>
<tr><td nowrap><td><input type="submit" value="投稿する"><input type="reset" value="中止"></td></tr>
</table></form><hr size=1>
$set{'message'}
EOM
	$header .= "・最大$set{'max_size'}KBまでアップロードできます<br>\n";
	if ($set{'max_mode'} == 0){$header .= "・最大$set{'max_tree'}スレッドが保存されます";}
	elsif($set{'max_mode'} == 1){$header .= "・レスを含め最大$set{'max_log'}件の投稿が記録されます";}
	my $t = 1;
	my $s = 0;
	while($t <= $lastpage){
		my $printbuff;
		my @buff;
		my $u = 0;
		while($u < $set{'pagelog'}){
			$u++;
			my $m = $s + $u - 1;
			push(@buff,$tmp[$m]);
			last if ($m == $tmp -1);
		}
		foreach my $value (@buff){
			my ($treeno,$allno,$dummy) = split(/<>/,$value);
			my ($disphost,$dispid,$i);
			unless ($treeno > 0){ next; }
			open(IN,"$set{'log_dir'}$treeno$set{'log_ext'}")|| &error(303,"$set{'log_dir'}$treeno$set{'log_ext'}");
			my $log = my @log = <IN>;
			close(IN);
			my $ry = $log - $set{'res_ry'} - 1;
			if ($set{'disp_ip'} == 1){
				$disphost = 1;
			}elsif($set{'disp_ip'} == 2){
				$dispid = 1;
			}
			foreach my $logvalue (@log){
				$i++;
				my ($reno,$no,$name,$email,$comment,$date,$host,$pass,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size) = split(/<>/,$logvalue);
				if ($reno == $no){
					$printbuff .= make_item($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,1);
					$printbuff .= "<TABLE border=0 cellpadding=2 summary=\"$no\"><TR><TD rowspan=2 width=40><BR></TD><TD valign=top nowrap><font color=\"#666666\">※レス$ry件省略 全て読むには返信ボタンを押してください</font></TD></TR></TABLE>\n" if($ry > 0);
				}else{
					if ($i < $ry + 2){ next; }
					$printbuff .= make_item($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,2);
				}
			}
		}
		my $footer = "<BR CLEAR=LEFT><hr size=1>";
		my $v = $t - 1;
		if ($t > 1){
			if($v == 1){
				$footer .= "<a href=\"$set{'base_html'}\">＜前の$set{'pagelog'}件</a>";
			}else{
				$footer .= "<a href=\"$set{'base_pre'}$v$set{'base_ext'}\">＜前の$set{'pagelog'}件</a>";
			}
		}
		$v += 2;
		my $n = 1;
		if ($t < $lastpage){ $footer .= " <a href=\"$set{'base_pre'}$v$set{'base_ext'}\">次の$set{'pagelog'}件＞</a>";}
		while($n <= $lastpage){
			if ($n == $t){
				$footer .= " [$n]";
			}else{
				if ($n == 1){
					$footer .= " <a href=\"$set{'base_html'}\">[$n]</a>";
				}else{
					$footer .= " <a href=\"$set{'base_pre'}$n$set{'base_ext'}\">[$n]</a>";
				}
			}
			$n++;
		}
		$footer .= "\n<BR CLEAR=LEFT><HR size=1>\n<div align=center><FORM METHOD=POST ACTION=\"$set{'base_cgi'}\"><span style='font-size:9pt'><input type=hidden name=mode value=delete>*投稿時のﾈｯﾄﾜｰｸ又は削除ｷｰが一致すれば削除出来ます<br>投稿No.<input type=text size=5 name=delno> 削除ｷｰ<input type=password size=5 name=delpass> <input type=submit value=\"削除\"></span></form></div>\n";
		$n = 1;
		$footer .= "</blockquote><div align=center><span style='font-size:9pt'><!-- $set{'ver'} --><a href=\"http://sugachan.dip.jp/download/\" target=\"_top\">imgbbs(仮)</a></span></div></body></html>";
		
		my $htmlfile;
		if($t == 1){
			$htmlfile = $set{'base_html'};
		}else{
			$htmlfile = "$set{'base_pre'}$t$set{'base_ext'}";
		}
		open(OUT,">$htmlfile") || &error(305,"$htmlfile");
		print OUT $header.$printbuff.$footer;
		close(OUT);
		chmod($set{'per_html'},"$htmlfile");

		$s += $set{'pagelog'};
		$t++;
	}
	
	while($t < 100){#掃除
		unlink("$set{'base_pre'}$t$set{'base_ext'}");
		$t++;
	}
}

#-------------------
#　返信フォーム作成
#  (refresh,更新ログno)
#-------------------
sub makeres{
	my ($i,$printbuff,$refno,$homedir);
	
	$refno = $_[0];
	unless($refno =~ /^\d+$/){ return; }

	open(IN,"$set{'log_dir'}$refno$set{'log_ext'}")|| &error(303,"$set{'log_dir'}$refno$set{'log_ext'}");
	my @tmp = <IN>;
	close(IN);
	
	$homedir = $set{'res_dir2'} if($set{'home'} =~ /^\./);
	
	my $header =<<"EOM";
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang=ja>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=$set{'jcode'}">
<META HTTP-EQUIV="CONTENT-SCRIPT-TYPE" CONTENT="text/javascript">
<script type="text/javascript">
<!--
var cookiename = "$set{'cookie_name'}";function getCookie(obj)
{var i, str;c = new Array(); p = new Array("", "", "");
str = document.cookie;c = str.split(";");
for (i = 0; i < c.length; i++) {if (c[i].indexOf(cookiename+"=") >= 0) {
p = (c[i].substr(c[i].indexOf("=")+1)).split("<>");break;}}
obj.name.value =  unescape(p[0]); obj.email.value = unescape(p[1]);
obj.pass.value =  unescape(p[2]);
return true;}
//-->
</script>
<title>$set{'title'}</title>
$set{'style_sheet'}
</head><body text="$set{'text'}" bgcolor="$set{'bgcolor'}" link="$set{'link'}" alink="$set{'link'}" vlink="$set{'vlink'}" onload="getCookie(document.Form);">
<blockquote>
<div align=right>[<a href="$set{'res_dir2'}$set{'base_html'}">掲示板に戻る</a>] [<a href="$homedir$set{'home'}" target="_top">ホーム</a>] [<a href="$set{'res_dir2'}$set{'base_cgi'}?mode=admin">管理用</a>]</div>
[ <a href="javascript:history.back()">戻る</a> ] No.$refno への返信
<form action="$set{'res_dir2'}$set{'base_cgi'}" method="POST" name="Form" enctype="multipart/form-data">
<table border=0 cellspacing=0 summary="投稿フォーム">
<tr>
  <td nowrap>おなまえ</td>
  <td><input type=text name="name" size=35 value="">
	<input type="hidden" name="mode" value="res">
	<input type="hidden" name="reno" value="$refno">
	<input type="hidden" name="jcode" value="漢字">
</td>
</tr>
<tr>
  <td nowrap>email</td>
  <td><input type="text" name="email" size=35 value=""></td>
</tr>
<tr><td nowrap>本文</td>
<td><textarea name=comment COLS=40 ROWS=3></textarea></td></tr>
<tr><td>画像選択</td><td><input type="file" name="upfile" size="30"> <input type="checkbox" name="textonly" value="on" checked>画像なし</td></tr>
<tr><td nowrap>削除キー</td><td><input type="password" size=8 name="pass" value=""></td></tr>
<tr><td nowrap><td><input type="submit" value="投稿する"><input type="reset" value="中止"></td></tr>
</table></form>
EOM
	my ($dispid,$disphost);
	if ($set{'disp_ip'} == 1){
		$disphost = 1;
	}elsif($set{'disp_ip'} == 2){
		$dispid = 1;
	}
	foreach my $value (@tmp){
		my ($reno,$no,$name,$email,$comment,$date,$host,$pass,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size) = split(/<>/,$value);
		$i++;
		if ($reno == $no){
			$printbuff .= make_item($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,3);
		}else{
			$printbuff .= make_item($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,4);
		}
	}
	
	my $footer = "<BR CLEAR=LEFT><hr size=1></blockquote>\n<div align=center><span style='font-size:9pt'><!-- $set{'ver'} --><a href=\"http://sugachan.dip.jp/download/\" target=\"_top\">imgbbs(仮)</a></span></div></body></html>";
	open(OUT,">$set{'res_dir'}res$refno$set{'res_ext'}");
	print OUT $header.$printbuff.$footer;
	close(OUT);
	chmod($set{'per_html'},"$set{'res_dir'}res$refno$set{'res_ext'}");
}

#----------------
# quit
#----------------
sub quit{
	print "Content-type: text/html\n\n";
	print <<"EOM";
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang=ja><head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=$set{'jcode'}">
<META HTTP-EQUIV="Refresh" CONTENT="1;URL=$set{'base_html'}">
EOM
	if ($in{'mode'} eq 'regist'||$in{'mode'} eq 'res'){
		print <<"EOM";
<META HTTP-EQUIV="CONTENT-SCRIPT-TYPE" CONTENT="text/javascript">
<script type="text/javascript">
<!--
setCookie();
function setCookie(tmp ,name,email,pass) {
	name = 	escape('$in{'org_name'}');
	email = escape('$in{'org_email'}');
	pass = 	escape('$in{'org_pass'}');
    tmp = "expires=Fri, 31-Dec-2030 23:59:59; ";
    document.cookie = "$set{'cookie_name'}="+name+"<>"+email+"<>"+pass+"; "+ tmp;
}
//-->
</script>
EOM
	}
	print <<"EOM";
<title>$set{'title'}</title></head>
<body text="$set{'text'}" bgcolor="$set{'bgcolor'}" link="$set{'link'}" alink="$set{'link'}" vlink="$set{'vlink'}">
<br><br><div align=center><font size="+1">しばらくお待ちください。<br><br>
<a href="$set{'base_html'}">ページが変わらないときはここをクリック</a></font><br>
</div>
</body></html>
EOM
exit;
}

#----------------
# ファイルチェック
# 書き込み
#----------------
sub filecheck{
	
	my $thumb_flag = 0;
	my $break_flag = 0;
	my $icon;

	$in{'random'} = int(rand(900000)) + 100000;
	if(-e "$set{'img_dir'}$in{'random'}.tmp"){$in{'random'}++;}
	if(-e "$set{'img_dir'}$in{'random'}.tmp"){ &error(204); }
	
	open (FILE,">$set{'img_dir'}$in{'random'}.tmp") || &error(202);
	binmode(FILE);
	eval{ while(my $read = read($in{'upfile'}, my $buff, 4096)){ print FILE $buff; }};
	close(FILE);
	chmod($set{'per_upfile'},"$set{'img_dir'}$in{'random'}.tmp");

	($in{'W'},$in{'H'},my $format,my $dummy,my $gif_flame) = GetImageSize("$set{'img_dir'}$in{'random'}.tmp",1);
	
	if($format < 0){
		if($set{'image_check'} == 1){ $break_flag = 1; }
		elsif($set{'image_check'} == 2){ &error(205);}
		$format = abs($format);
	}
	if($format == 1){ $in{'ext'} = '.gif';}
	elsif($format == 2){ $in{'ext'} = '.jpg';}
	elsif($format == 3){ $in{'ext'} = '.png';}
	elsif((-e $set{'mime'}) && $in{'type'}){ ($in{'ext'},$icon) = mimefind($in{'type'},$in{'upfile'});}
	else{ &error(108);}
	
	if(-e "$set{'img_dir'}$in{'time'}$in{'ext'}"){$in{'time'}++;}
	if(-e "$set{'img_dir'}$in{'time'}$in{'ext'}"){$in{'time'}++;}
	if(-e "$set{'img_dir'}$in{'time'}$in{'ext'}"){$in{'time'}++;}
	if(-e "$set{'img_dir'}$in{'time'}$in{'ext'}"){ &error(201); }
	
	rename("$set{'img_dir'}$in{'random'}.tmp","$set{'img_dir'}$in{'time'}$in{'ext'}");
	
	my $size = (-s "$set{'img_dir'}$in{'time'}$in{'ext'}");
	
	my $key;

	if($in{'W'} > 0 && $in{'H'} > 0 ){
		$key = $set{'max_w'} / $in{'W'};
		$key = $set{'max_h'} / $in{'H'} if ($set{'max_h'} / $in{'H'} < $key);
		$in{'newW'} = int($in{'W'} * $key);
		$in{'newH'} = int($in{'H'} * $key);
	}elsif(!$in{'ext'}){
		unlink("$set{'img_dir'}$in{'time'}$in{'ext'}");
		&error(203);
	}
	if ($in{'W'} > $in{'newW'}){
		if($set{'use_thumb'} == 1){
			$thumb_flag = 1;
		}
	}elsif($in{'ext'}){
		$in{'newH'} = $in{'H'};
		$in{'newW'} = $in{'W'};
		if($set{'use_thumb'} == 1 && $gif_flame > 1 && $size > $set{'max_gifanime'} * 1024 ){
			$thumb_flag = 1;
		}
	}
	
	if($thumb_flag && !$break_flag){
		eval{
			require $set{'resize'};
			&imgbbs::imgresize("$set{'img_dir'}$in{'time'}$in{'ext'}","$set{'thumb_dir'}$in{'time'}s.jpg",$in{'newW'},$in{'newH'},75,1);
		};
	}elsif($break_flag){
		my $ext;
		($in{'newW'},$in{'newH'},my $fsno) = GetImageSize($set{'break_icon'});
		if($fsno == 1){ $ext = '.gif';}
		elsif($fsno == 2){ $ext = '.jpg';}
		elsif($fsno == 3){ $ext = '.png';}
		else{ &error(205); }
		&error(205) if(!copy("$set{'break_icon'}","$set{'thumb_dir'}$in{'time'}s$ext"));
	}elsif($in{'ext'}){
		my $ext;
		
		($in{'newW'},$in{'newH'},my $fsno) = GetImageSize($icon);
		if($fsno == 1){ $ext = '.gif';}
		elsif($fsno == 2){ $ext = '.jpg';}
		elsif($fsno == 3){ $ext = '.png';}
		copy("$icon","$set{'thumb_dir'}$in{'time'}s$ext");
	}
	$in{'size'} = $gif_flame.'ﾌﾚｰﾑ ' if ($gif_flame > 1 && $set{'disp_gifframe'});
	$in{'size'} .= int($size/1024)."KB";
}

sub make_item{
	my ($no,$i,$name,$email,$comment,$date,$host,$id,$sub,$time,$ext,$W,$H,$newW,$newH,$size,$dispid,$disphost,$itemmode) = @_;
	my ($buff,$disp);
	unless($itemmode == 1||$itemmode == 2||$itemmode == 3||$itemmode == 4){ return; }

	$comment =~ s/http:\/\/([\w\$\#\~\.\,\/\-\?\=\&\+\:\@\%\;]+)/<a href=\"http:\/\/$1\" target\="\_blank\">http:\/\/$1<\/a>/g if($set{'autolink'});

	if($dispid){ $disp = "<small>($id)</small>";}
	if($disphost){ $disp .= "<small>[$host]</small>"; }
	if($name =~ /^(.*)<\/b>◆(.*)<b>$/){
		my $tmpname = $1;
		my $tmptrip = $2;
		if($tmpname && $tmptrip){
			$name = "<b>$tmpname</b>◆$tmptrip";
		}else{
			$name = "◆$tmptrip";
		}
	}else{
		$name = "<b>$name</b>";
	}
	$name = "<a href=\"mailto:$email\">$name</a>" if ($email);
	$name = "<FONT COLOR=\"$set{'name'}\">$name</FONT>";
	my ($img,$msg,$resdir,$thumb);
	$resdir = $set{'res_dir2'} if($itemmode == 3||$itemmode == 4);
	
	if (-e "$set{'thumb_dir'}${time}s$ext"){ $thumb = "$set{'thumb_dir'}${time}s$ext"; }
	elsif(-e "$set{'thumb_dir'}${time}s.jpg"){ $thumb = "$set{'thumb_dir'}${time}s.jpg"; }
	elsif(-e "$set{'thumb_dir'}${time}s.png"){ $thumb = "$set{'thumb_dir'}${time}s.png"; }
	elsif(-e "$set{'thumb_dir'}${time}s.gif"){ $thumb = "$set{'thumb_dir'}${time}s.gif"; }
	
	if($thumb && $ext =~ /(jpe?g|gif|png)/){ $img = "<a href=\"$resdir$set{'img_dir'}$time$ext\" target=\"_blank\">$time$ext (${W}x${H} $size) <font size=-1>[サムネイル]</font><img src=\"$resdir$thumb\" border=0 align=left hspace=12 width=$newW height=$newH alt=\"$time$ext\"></a>\n"; }
	elsif($thumb){ $img = "<a href=\"$resdir$set{'img_dir'}$time$ext\" target=\"_blank\">$time$ext ($size)<img src=\"$resdir$thumb\" border=0 align=left hspace=12 width=$newW height=$newH alt=\"$time$ext\"></a>\n"; }
	elsif($ext =~ /(jpg|gif|png)/){ $img = "<a href=\"$resdir$set{'img_dir'}$time$ext\" target=\"_blank\">$time$ext (${W}x${H} $size)<img src=\"$resdir$set{'img_dir'}$time$ext\" border=0 align=left hspace=12 width=$newW height=$newH alt=\"$time$ext\"></a>\n"; }
	elsif(-e "$set{'img_dir'}${time}$ext"){$img = "<a href=\"$resdir$set{'img_dir'}$time$ext\" target=\"_blank\">$time$ext ($size)</a>\n";}
	
	$sub = "<FONT COLOR=\"$set{'subject'}\"><B>$sub</B></FONT>" if($sub);
	$msg = " <a href=\"$set{'res_dir'}res$no$set{'res_ext'}\" title=\"$noへの返信\">返信</a>" if ($itemmode == 1);
	if($itemmode == 1 || $itemmode == 3){
		$buff =<<"EOM"
<BR CLEAR=LEFT><HR size=1>$img
<BR><BR>$i $sub $name $disp $date No.$no$msg
<BLOCKQUOTE>$comment</BLOCKQUOTE>
EOM
	}elsif($itemmode == 2||$itemmode == 4){
		$buff =<<"EOM";
<TABLE border=0 summary="$noの投稿"><TR><TD NOWRAP ALIGN=RIGHT VALIGN=TOP>&gt;&gt;</TD><TD bgcolor="$set{'tablecolor'}">
$i $name $disp $date No.$no
<div>$img</div><BLOCKQUOTE>$comment</BLOCKQUOTE></TD></TR></TABLE>
EOM
	}
	return $buff;
}

#----------------
# 関数
#----------------
sub conv_date{
	if ($_[0] < 1000000000){return;}
	my @date = localtime($_[0]);
	$date[5] -= 100;
	$date[4]++;
	if ($date[5]  < 10) { $date[5] = "0$date[5]" ; }
	if ($date[4]  < 10) { $date[4] = "0$date[4]" ; }
	if ($date[3]  < 10) { $date[3] = "0$date[3]" ; }
	if ($date[2]  < 10) { $date[2] = "0$date[2]" ; }
	if ($date[1]  < 10) { $date[1] = "0$date[1]" ; }
	if ($date[0]  < 10) { $date[0] = "0$date[0]" ; }
	return ("$date[5]/$date[4]/$date[3],$date[2]:$date[1]:$date[0]");

}

sub GetImageSize{
	my ($src,$check) = @_;
	my $buff;	my $gif_frame;
	my $width = 0;	my $height = 0;
	my $formatno = 0;
	my $flag = 0;

	open(IMG,$src)|| return 0;
	binmode(IMG);
	read(IMG, $buff, 4);
	
	if($buff =~ /^GIF/){
		$gif_frame = 0;
		seek(IMG, 6, 0);
		read(IMG, my $buff, 4);
		$width = unpack("v",substr($buff,0,2));
		$height = unpack("v",substr($buff,2,2));
		while (<IMG>){ $gif_frame++ if(/\x21\xf9\x04/); }
		seek(IMG, -1, 2);
		read(IMG,$buff,1);
		$flag = 1 if($buff =~ /^\x3b$/);
		$formatno = 1;
	}elsif($buff =~ /PNG/){
		seek(IMG, 16, 0);
		read(IMG, my $buff, 8);
		$width = unpack("N", substr($buff, 0, 4));
		$height = unpack("N", substr($buff, 4, 8));
		seek(IMG, -8, 2);
		read(IMG,$buff,8);
		$flag = 1 if($buff =~ /^\x49\x45\x4e\x44\xae\x42\x60\x82$/);
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
		seek(IMG, -2, 2);
		read(IMG,$buff,2);
		$flag = 1 if($buff =~ /^\xff\xd9$/);
		$formatno = 2;
	}
	close(IMG);
	if($formatno < 1 || $width < 1 || $height < 1){ return 0; }
	if(!$flag && $check){ $formatno = -$formatno; }
	return ($width,$height,$formatno,"width=\"$width\" height=\"$height\"",$gif_frame);
}

sub copy{
	my($src,$dst) = @_;
	open(IN,$src)||return 0;
	open(OUT,">$dst")||return 0;
	binmode(IN);
	binmode(OUT);
	while(<IN>) { print OUT; }
	close(OUT);
	close(IN);
	if(-e "$dst"){ return 1; }
	return 0;
}
#----------------
# 初期設定
#----------------
sub init{
	open(OUT,">$set{'log_file'}")||&error(302);
	print OUT "0<>0<>0<>1\n";
	close(OUT);
	chmod($set{'per_logfile'},$set{'log_file'});

	unless(-e "$set{'log_error'}"){
		open(OUT,">$set{'log_error'}");
		close(OUT);
		chmod($set{'per_logfile'},$set{'log_error'});
	}
	unless(-e "$set{'log_delete'}"){
		if ($set{'delete_log_level'} >= 1){
			open(OUT,">$set{'log_delete'}");
			close(OUT);
			chmod($set{'per_logfile'},$set{'log_delete'});
		}
	}
	unless (-d "$set{'log_dir'}"){
		mkdir("$set{'log_dir'}",$set{'per_dir'});
		chmod($set{'per_dir'},"$set{'log_dir'}");
		open(OUT,">$set{'log_dir'}index.html");
		close(OUT);
		chmod($set{'per_html'},"$set{'log_dir'}index.html");
	}
	unless (-d "$set{'img_dir'}"){
		mkdir("$set{'img_dir'}",$set{'per_dir'});
		chmod($set{'per_dir'},"$set{'img_dir'}");
		open(OUT,">$set{'img_dir'}index.html");
		close(OUT);
		chmod($set{'per_html'},"$set{'img_dir'}index.html");
	}
	unless (-d "$set{'thumb_dir'}"){
		mkdir("$set{'thumb_dir'}",$set{'per_dir'});
		chmod($set{'per_dir'},"$set{'thumb_dir'}");
		open(OUT,">$set{'thumb_dir'}index.html");
		close(OUT);
		chmod($set{'per_html'},"$set{'thumb_dir'}index.html");
	}
	unless (-d "$set{'res_dir'}"){
		mkdir("$set{'res_dir'}",$set{'per_dir'});
		chmod($set{'per_dir'},"$set{'res_dir'}");
		open(OUT,">$set{'res_dir'}index.html");
		close(OUT);
		chmod($set{'per_html'},"$set{'res_dir'}index.html");
	}
}
#----------------
# mimetype判別
#----------------
sub mimefind{
	my ($src,$orgname) = @_;
	
	open(IN,$set{'mime'})||return 0;
	my @mime = <IN>;
	close(IN);
	foreach my $value (@mime){ #mimetype で判別
		if($value =~ /^(\#|\s)/){ next; }
		chomp($value);
		my @tmp = split(/\t/,$value);
		my @ext = split(/,/,$tmp[0]);
		if($src =~ /$tmp[1]/){ return (".$ext[0]",$tmp[2]);}
	}
	
	foreach my $value (@mime){ #mimetypeで検出できなかった場合
		if($value =~ /^(\#|\s)/){ next; }
		chomp($value);
		my @tmp = split(/\t/,$value);
		my @ext = split(/,/,$tmp[0]);
		foreach my $value2 (@ext){
			if($orgname =~ /\.$value2$/){ return (".$ext[0]",$tmp[2]);}
		}
	}
	return  0;
}

#----------------
#　エラー処理
#----------------
sub error{
	my ($no,$message,$flag);
	$no = $_[0];
	$flag = 0;
#禁止不正処理関連 100～
#画像関連 200～
#ログ関連 300～
#削除関連 400～
#admin操作関連 500～

#101 禁止ホスト
#102 禁止ファイル
#103 名前長すぎ
#104 メール長すぎ
#105 コメント長すぎ
#106 POSTサイズ大きすぎ
#107 mode指定無し
#108 画像無し
#109 レス番号不正
#110 コメントが無い
#111 レス早すぎ
#112 題名長すぎ
#113 改行が多すぎ
#114 漢字コードがおかしい

#201 画像書き込めず-ファイルが既に存在
#202 画像書き込めず
#203 画像サイズ取得失敗
#204 一時ファイルが書きこめない
#205 画像が破損している

#301 メインログ読み込めず
#302 メインログ書き込めず
#303 サブログ読み込めず 引数にスレファイル名
#304 サブログ書き込めず 引数にスレファイル名
#305 ログHTML書き込めず 引数にスレファイル名
#306 ログディレクトリ読み込めず

#401 削除記事番号見つからず
#402 削除パス一致せず

#501 管理者パスワード一致せず
#502 削除モード異常

	if ($no == 101){
		$message = '投稿禁止ホストです';
	}elsif($no == 102){
		$message = '画像ファイル(JPEG,GIF,PNG)以外はUPできません';
	}elsif($no == 103){
		$message = '名前が長すぎです';
	}elsif($no == 104){
		$message = 'メールが長すぎです';
	}elsif($no == 105){
		$message = 'コメントが長すぎです';
	}elsif($no == 106){
		$message = 'POSTサイズが大きすぎです';
	}elsif($no == 99){
		$message = '定義されていないモードです';
	}elsif($no == 108){
		$message = '添付画像または添付ファイルがないか<br>許可されていないファイル形式です';
	}elsif($no == 109){
		$message = 'その投稿番号にはレスはつけることができません';
	}elsif($no == 110){
		$message = 'コメントがありません';
	}elsif($no == 111){
		$message = '投稿間隔をあけてください';
	}elsif($no == 112){
		$message = '題名が長すぎです';
	}elsif($no == 113){
		$message = '改行が多すぎです'." $_[1]行くらい";
		undef $_[1];
	}elsif($no == 114){
		$message = '漢字コードが変です '.$set{'jcode'}." を使ってください";
	}elsif($no == 201){
		$message = 'アップロードに失敗しました';
	}elsif($no == 202){
		$message = 'アップロードに失敗しました';
	}elsif($no == 203){
		$message = '許可されていないファイル形式です';
	}elsif($no == 204){
		$message = '一時ファイルの書き込みに失敗しました';
	}elsif($no == 205){
		$message = '画像が破損しています';
	}elsif($no == 301){
		$message = 'メインログの読み込みに失敗しました';
	}elsif($no == 302){
		$message = 'メインログの書き込みに失敗しました';
	}elsif($no == 303){
		$message = 'ログdatの読み込みに失敗しました';
		$flag = 1;
	}elsif($no == 304){
		$message = 'ログdatの書き込みに失敗しました';
	}elsif($no == 305){
		$message = 'ログHTMLの書き込みに失敗しました';
		$flag = 1;
	}elsif($no == 306){
		$message = 'ログディレクトリの読み込みに失敗しました';
	}elsif($no == 401){
		$message = '削除記事番号が見つかりません';
	}elsif($no == 402){
		$message = '削除キーが一致しません';
	}elsif($no == 501){
		$message = '管理者パスワードが一致しません';
	}elsif($no == 502){
		$message = '削除モードが異常です';
	}else{
		$message = '定義されていないエラーです';
	}
	
	if ($flag){
		open (OUT,">>$set{'log_error'}");
		print OUT conv_date(time)." $message $_[1]\n";
		close(OUT);
	}else{
		eval {close($in{'upfile'});};
		unlink($in{'tmpfile'});
		unlink("$set{'img_dir'}$in{'random'}.tmp");
		unless($_[0] == 201) { unlink("$set{'img_dir'}$in{'time'}$in{'ext'}"); }
		if($set{'error_level'} == 1 && $no =~ /^\d+$/ && $no > 100){
			open (OUT,">>$set{'log_error'}");
			print OUT conv_date(time)." $in{'host'} $message $_[1]\n";
			close(OUT);
		}
		print "Content-type: text/html\n\n";
		print <<EOM;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang=ja>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=$set{'jcode'}">
<title>$set{'title'}</title>
</head>
<body text="$set{'text'}" bgcolor="$set{'bgcolor'}" link="$set{'link'}" alink="$set{'link'}" vlink="$set{'vlink'}">
<br><div align=center><font size="+1"><br>$message<br><br>[ <a href="javascript:history.back()">戻る</a> ]</font></div>
</body></html>
EOM
		exit;
	}
}
