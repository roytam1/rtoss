#!/usr/local/bin/perl

#┌─────────────────────────────────
#│  JOYFUL NOTE v1.96 (2006/01/18)
#│  Copyright (c) KentWeb
#│  webmaster@kent-web.com
#│  http://www.kent-web.com/
#└─────────────────────────────────
$ver = 'JoyfulNote v1.96';
$resizever = '15:21 2004/01/24';
$rthackver = 'RTHack 20070614';
#┌─────────────────────────────────
#│ [注意事項]
#│ 1. このスクリプトはフリーソフトです。このスクリプトを使用した
#│    いかなる損害に対して作者は一切の責任を負いません。
#│ 2. 設置に関する質問はサポート掲示板にお願いいたします。
#│    直接メールによる質問は一切お受けいたしておりません。
#│ 3. このスクリプトは、method=POST 専用です。	
#│ 4. 同梱のアイコンで、以下のファイルの著作権者は以下のとおりです。
#│    home.gif : mayuRinさん
#│    clip.gif : 牛飼いとアイコンの部屋さん
#└─────────────────────────────────
#
# 【ファイル構成例】
#
#  public_html (ホームディレクトリ)
#      |
#      +-- joyful / joyful.cgi [705]
#            |      joyful.log [606]
#            |      count.dat  [606]
#            |      jcode.pl   [604]
#            |      cgi-lib.pl [604]
#            |      pastno.dat [606]
#            |
#            +-- img  [707] / home.gif, bear.gif, ...
#            |
#            +-- lock [707] /
#            |
#            +-- past [707] / 1.dat [606] ...

#-------------------------------------------------
#  設定項目
#-------------------------------------------------
require "./config.inc.cgi";
use PTE;
$PTE=new PTE($template);

# Admin's note (below the form of index)
if($storage_limit) {
	$msize=$maxsize/1024;
	$currusage=&totalsize;
	$dispusage=sprintf("%.2f",$currusage / 1024);
	$admin_note=<<EON;
<li>Max file size: $msize KB</li>
<li>Current usage: $dispusage KB / $storage_amount KB</li>
EON
}
$admin_note.=$board_note;

#Import spamdata.cgi
open(SD,"spamdata.cgi"),$NGWords=<SD>.$NGWords, close(SD) if (-e "spamdata.cgi");

#-------------------------------------------------
#  設定完了
#-------------------------------------------------
# get the language
$tmplang=$ENV{'QUERY_STRING'};
$tmplang=~/lang=[A-Za-z]{2}/i;
$langstr=$&;
$langstr='lang=' if($langstr eq '');
($lang,$lang)=split(/=/,$langstr);

$dest=$htmlfile?"$htmlfile":"$script?$langstr";
# ライブラリ取込
require './joyful-str.cht.pl';
require './joyful-str.jpn.pl' if($lang eq 'ja');
require './cgi-lib.pl';
&decode;
require './resize.pl' if(-e './resize.pl');

# メイン処理
&axscheck;
if ($mode eq "howto") { &howto; }
elsif ($mode eq "find") { &find; }
elsif ($mode eq "usr_del" || $mode eq "usr_del_img") { &usr_del; }
elsif ($mode eq "usr_edt") { &usr_edt; }
elsif ($mode eq "regist") { &regist; }
elsif ($mode eq "res") { &res_form; }
elsif ($mode eq "threads") { &th_table(1); }
elsif ($mode eq "admin") { &admin; }
elsif ($mode eq "past") { &past; }
elsif ($mode eq "check") { &check; }
elsif ($mode eq "remake") { if ($htmlfile) { $outputhtml=1;$backtoindex=1;&html_log;$outputhtml=0;exit;} }
elsif ($mode eq "chibi") { &chibi; }
elsif ($mode eq "chibihelp") { &chibi("help"); }
&html_log if (!$htmlfile || defined $page);
if ($htmlfile && !(-e $htmlfile)) { $outputhtml=1;$backtoindex=1;&html_log;$outputhtml=0;exit;}
else { &backtoIndex; }

#----------------#
#  アクセス制限  #
#----------------#
sub axscheck {
	return if($onlyblockonpost && $mode ne "regist");
	# ホスト名取得
	$host = $ENV{'REMOTE_HOST'};
	$addr = $ENV{'REMOTE_ADDR'};
	$addr = $ENV{'HTTP_X_FORWARDED_FOR'} if($ENV{'HTTP_X_FORWARDED_FOR'} ne '');
	if ($gethostbyaddr && ($host eq "" || $host eq $addr)) {
		$host = gethostbyaddr(pack("C4", split(/\./, $addr)), 2);
	}
	if ($host eq "") { $host = $addr; }

	my($flg,$cnt)=0;
	foreach ( split(/\s+/, $deny) ) {
		s/\./\\\./g;
		s/\*/\.\*/g;
		if ($host =~ /$_/i || $addr =~ /$_/) { $flg=1; last; }
	}

	$bypressIPQ=1 if($in{'name'} =~ /fusianasan/ || $in{'name'} =~ /mokorikomo/ );

	if($extIPQ && !$bypressIPQ && $addr ne "127.0.0.1") {
		my @queries = ( 'list.dsbl.org','bbx.2ch.net','dnsbl.ahbl.org','niku.2ch.net','virus.rbl.jp','ircbl.ahbl.org','tor.ahbl.org' );
		foreach ( @queries ) {
			$ip0 = join '.', unpack 'C4', gethostbyname(sprintf "%s.".$_, join '.', reverse split /\./, $addr);
			if($ip0 =~ /^127/){  $flg=1; last; }
			$cnt++;
			last if($cnt>=$extIPQ);
		}
	}

	if ($flg) { &error($jnstr_addrblock); }
}

#--------------#
#  禁止ワード  #
#--------------#
sub NGcheck {
	my($word) = @_;

	my($flg);
	foreach ( split(/,+/, $NGWords) ) {
		if (index($word,$_) >= 0) { $flg=1;$out=$_; last; }
	}
	if ($flg) { &error($jnstr_NGWord."(".$out.")"); }
}

#--------------#
#  記事表示部  #
#--------------#
sub html_log {
	my($ipt,$wh,$i,$flag,$size,$attech);

	# ヘッダを出力
	&header;

	# カウンタ処理
	if ($counter) { &counter; }

	&showtitle;
	&showmenu();

	# 投稿フォーム
	print &form();

	# スレッド一覧
	if ($th_log || ($showres>-1)) { &th_table; }
	&loadlog($logfile) if (!defined $logs);

	&getOversize;

	# 記事展開
	$i=0;
	$flag=0;
	$i_res=0;
	$res_flag=0;
	$reschar=$gazouliked?$gazouResChar:'&emsp;';

	my %args=('{$th}'=>());

	for ($i=0;$i<=$#torder;$i++) {
		if ($i < $page * $p_log) { next; }
		if ($i > ($page + 1) * $p_log) { $i=$#torder;last; }
		for (my $j=0;$j<=$#{$trees{$torder[$i]}};$j++) {
			($no,$reno,$date,$name,$mail,$sub,$comment,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$trees{$torder[$i]}[$j]}};
			
			# 題名の長さ
			if ($sub_len && length($sub) > $sub_len*2) {
				$sub = substr($sub,0,$sub_len*2);
				$sub .= "...";
			}

			$comment =~ s/([>]|^)(&gt;[^<]*)/$1<span class='quoted'>$2<\/span>/g;
			if ($mail) { $name = "<a href=\"mailto:$mail\">$name</a>"; }
			if ($home_icon && $url) { $url = "<a href=\"http://$url\" target='_blank'><img src=\"$imgurl$IconHome\" border=0 align=top alt='HomePage'></a>"; }
			elsif (!$home_icon && $url) { $url = "&lt;<a href=\"http://$url\" target='_blank'>HOME</a>&gt;"; }
			
			# 添付ファイルが存在する場合
			$attech=attechmentProc($tail,$w,$h);

			# 自動リンク
			if ($autolink) { &auto_link($comment); }

			if(!$j) { # First post = thread top
				my ($overflow,$bekill,$hidepost);
				$hidepost=($showres>-1 && $res_log[$i]>$showres)?$res_log[$i]-$showres:0;
				$overflow=$hidepost?"<span class='warn_txt'>$jnstr_overflow_pre$hidepost$jnstr_overflow_post</span><br/>\n":'';
				$bekill=$oversize{$no}?"<span class='warn_txt2'>$jnstr_size_warn</span><br/>\n":'';
				push @{$args{'{$th}'}},{'{$NAME_TEXT}'=>"$jnstr_poster$jnstr_colon",'{$TIME_TEXT}'=>"$jnstr_postdate$jnstr_colon",'{$NO}'=>$no,
					'{$ATTECH}'=>$attech,'{$SUB}'=>$sub,'{$NAME}'=>$name,'{$TIME}'=>$date,'{$REPLYBTN}'=>"<a href=\"$script?mode=res&amp;no=$no&amp;$langstr\">$jnstr_reslink</a>\n",
					'{$PROCBTN}'=>"<a href=\"#proc\" OnClick=\"document.Proc.no.value='$no';\">$jnstr_proclink</a>\n",'{$COL}'=>$color,'{$COM}'=>$comment,'{$WARN_BEKILL}'=>$bekill,'{$WARN_HIDEPOST}'=>$overflow,'{$res}'=>()};
			} else {
				if($j<$res_log[$i]-$showres+1 && $showres>0){
					$j+=$res_log[$i]-$showres-1;
					next;
				}

				$bekill=$oversize{$no}?"<span class='warn_txt2'>$jnstr_size_warn</span><br/>\n":'';
				push @{$args{'{$th}'}[$#{$args{'{$th}'}}]{'{$res}'}},{'{$NAME_TEXT}'=>"$jnstr_poster$jnstr_colon",'{$TIME_TEXT}'=>"$jnstr_postdate$jnstr_colon",'{$NO}'=>$no,
					'{$ATTECH}'=>$attech,'{$SUB}'=>$sub,'{$NAME}'=>$name,'{$TIME}'=>$date,'{$PROCBTN}'=>"<a href=\"#proc\" OnClick=\"document.Proc.no.value='$no';\">$jnstr_proclink</a>\n",
					'{$COL}'=>$color,'{$COM}'=>$comment,'{$WARN_BEKILL}'=>$bekill};
			}
		}
	}

	print $PTE->ParseBlock('threads',\%args);

	$next = $page + 1;
	$back = $page - 1;

	$p_flag=0;
	print "<blockquote><table cellpadding=0 cellspacing=0><tr>\n";
	if ($back >= 0) {
		$p_flag=1;
		$dest2=$htmlfile && $back==0?"$dest":"$script";
		print "<td><form action=\"$dest2\">\n";
		print "<input type=hidden name=page value=\"$back\">\n";
		print "<input type=hidden name=lang value=\"$lang\">\n";
		print "<input type=submit value=\"$jnstr_prevpage_pre$p_log$jnstr_post_n\"></form></td>\n";
	}
	if ($next * $p_log < $i) {
		$p_flag=1;
		print "<td><form action=\"$script\">\n";
		print "<input type=hidden name=page value=\"$next\">\n";
		print "<input type=hidden name=lang value=\"$lang\">\n";
		print "<input type=submit value=\"$jnstr_nextpage_pre$p_log$jnstr_post_n\"></form></td>\n";
	}
	# ページ移動ボタン表示
	if ($p_flag) {
		print "<td width=10></td><td>\n";
		$y=0;
		while ($i > 0) {
			if ($page == $y) { print "<b>[$y]</b>\n"; }
			else { $dest2=$htmlfile && $y==0?"$dest":"$script"; print "[<a href=\"$dest2?page=$y&amp;$langstr\">$y</a>]\n"; }
			$y++;
			$i = $i - $p_log;
		}
		print "</td>\n";
	}
	print "<td></td></tr></table></blockquote>\n";

	&procform();
	&footer;
	
	exit if(!$outputhtml);
}
sub procform {
	if ($_[0] ne "") {$val=" value=$_[0]";}
	print "<div align=center>\n";
	print "<a name=\"proc\"></a><form action=\"$script?$langstr\" method=\"POST\" name=\"Proc\">\n";
	print "<font color=\"$t_color\">- $jnstr_form_pre -</font><br>\n";
	print "$jnstr_proc <select name=mode>\n";
	print "<option value=usr_edt>$jnstr_usr_edt\n";
	print "<option value=usr_del>$jnstr_usr_del\n";
	print "<option value=usr_del_img>$jnstr_usr_del_img</select>\n";
	print "$jnstr_resno <input type=text name=no size=3$val>\n";
	print "$jnstr_delkey <input type=password name=pwd size=4 maxlength=8>\n";
	print "<input type=submit value=\"$jnstr_submit\"></form></div>\n";
}

sub showtitle {
	# タイトル部
	print "<div align='center'>\n";
	if ($banner1 ne "<!-- 上部 -->") { print "$banner1<p>\n"; }
	if ($t_img eq '') {
		print "<b style=\"font-size:$t_size;color:$t_color;\">$title</b>\n";
	} else {
		print "<img src=\"$t_img\" width=\"$t_w\" height=\"$t_h\" alt=\"$title\">\n";
	}
}

sub showmenu {
	# メニュー部
	print "<hr width=\"90%\">\n";
	if ($_[0] ne "") {print $_[0];}
	else{
		print "[<a href=\"$homepage\" target=\"_top\">$jnstr_backtotop</a>]\n";
		print "[<a href=\"$script?mode=howto&amp;$langstr\">$jnstr_howto</a>]\n";
		print "[<a href=\"$script?mode=find&amp;$langstr\">$jnstr_find</a>]\n";
		print "[<a href=\"$script?mode=past&amp;$langstr\">$jnstr_pastlog</a>]\n" if ($pastkey);
		print "[<a href=\"$script?mode=admin&amp;$langstr\">$jnstr_admin</a>]\n";
		print "[<a href=\"$dest\">$jnstr_reload</a>]\n";
	}
	print "<hr width=\"90%\"></div>\n";
}

sub footer {
		# 著作権表示部（削除改変不可）
	print "<div align=center>$banner2<p><!-- $ver --><!-- $resizever --><!-- $rthackver -->\n";
	print "<span style='font-size:10px;font-family:Verdana,Helvetica,Arial'>\n";
	print "- <a href='http://www.kent-web.com/' target='_top'>Joyful Note</a> + <a href='http://sugachan.dip.jp/download/' target='_top'>resize</a> + 中文化 by RT (based on Makoto) + <a href='http://scrappedblog.blogspot.com/'>改惡 + PTE</a> -<br>- <a href='http://w-space.net/subcon/cgi.htm'>&quot;Joyful Note + YY-BOARD + alpha&quot;</a> + <a href=\"$script?mode=chibi&amp;$langstr\">ちびぼーど風VIEW</a> -\n";
	print "</span></div>\n</body>\n</html>\n";
	
	if ($outputhtml) {
		select($cgiout);
		close(HOUT);
		&backtoIndex if ($backtoindex);
	}
}

sub backtoIndex {
	print "Content-type: text/html\n\n";
	print "<meta http-equiv=\"refresh\" content=\"0; url=$htmlfile?\" />";
}
sub getOversize {
	undef(%oversize);
	undef(%deloversize);
	&loadlog($logfile) if (!defined $logs);
	@descpodr=sort {$b <=> $a} (@porder);
	foreach (@descpodr) {
		($no,$reno,$date,$name,$mail,$sub,$comment,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$_}};

		if ($tail && -e "${imgdir}$ftime$tail") {
			$size += -s "${imgdir}$ftime$tail";
			$size += -s "${imgdir}$ftime"."s.jpg" if(-e "${imgdir}$ftime"."s.jpg");
		}
		if($size>$storage_amount*1024*0.85 && $tail) {$oversize{$no}=1;}
		if($size>$storage_amount*1024*0.95 && $tail) {$deloversize{$no}=1;}
	}
}
#--------------------#
#  ちびぼーど風VIEW  #
#--------------------#

sub chibi {
	my %args=('{$itv_time}'=>$itv_time,'{$speed}'=>$speed,'{$title}'=>$title,
		'{$top_mes}'=>$top_mes,'{$scroll}'=>$scroll,'{$script}'=>$script,'{$dest}'=>$dest,
		'{$target_win}'=>$target_win,'{$jnstr_chibi_gotobbs}'=>$jnstr_chibi_gotobbs,
		'{$jnstr_reload}'=>$jnstr_reload,'{$cb_neme}'=>$cb_neme,'{$langstr}'=>$langstr,
		'{$reload}'=>($data_re_load_time && $_[0] ne "help")?"  <meta http-equiv=\"refresh\" content=\"$data_re_load_time; url=$script?mode=chibi\" />":'');

	if($_[0] eq 'help') {
		my @mesg = ({'{$i}'=>'0','{$bgcolor}'=>'#d3d7d4','{$mesg}'=>"    <h1>$cb_neme $cb_ver</h1> <p style='color: #4f5555;'>$jnstr_chibi_showpost_pre <strong>$look_cnt</strong>$jnstr_chibi_showpost_post</p>"},
			{'{$i}'=>'1','{$bgcolor}'=>'#afb4db','{$mesg}'=>"<span style='color:red'>■</span>$jnstr_chibi_rights_1"},
			{'{$i}'=>'2','{$bgcolor}'=>'#abc88b','{$mesg}'=>"<span style='color:red'>■</span>$jnstr_chibi_rights_2"},
			{'{$i}'=>'3','{$bgcolor}'=>'#f7acbc','{$mesg}'=>"<span style='color:red'>■</span>$jnstr_chibi_rights_3"},
			{'{$i}'=>'4','{$bgcolor}'=>'#6f599c','{$mesg}'=>"<span style='color:red'>■</span>$jnstr_chibi_notice"});
		%args = (%args, ('{$jnstr_back}'=>$jnstr_back,'{$help}'=>1,'{$mes}'=>\@mesg));
	} else {
		my @mesg = ();

		# ■記事を展開
		$times = time;

		$i = 1;
		&loadlog($logfile) if (!defined $logs);
		@descpodr=sort {$b <=> $a} (@porder);
		foreach (@descpodr) {
	      ($no,$reno,$date,$name,$mail,$sub,$comment,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$_}};

	    # ■新着チェック
		    if ($tim > $times - 60 * 60 * $newh) {
		      $newimg = "<img src=\"$imgurl$newga\" alt=\"NEW\">";
		    } else {
		      $newimg = "■";
		    }

	    # ■日付
		    $date =~ s/ID.*//g;
		    $date =~ s/\(.*\)//g;
	    
	    # ■メール
		    if ($mail) { $name = "<a href=\"mailto:$mail\" title=\"Mail to\">$name</a>"; }
	    
	    # ■ＵＲＬ
		    if ($url and $home_icon) {
		      $url = "<a href=\"http://$url\" target=\"_blank\"><img src=\"$imgurl$IconHome\" align=top alt=\"HomePage\"></a>";
		    } elsif ($url) {
		      $url = "<a href=\"http://$url\" target=\"_blank\">URL</a>";
		    }
	    
	    # ■題名
		    if ($sub_len && length ($sub) > $sub_len * 2) {
		      $sub = substr ($sub,0,$sub_len * 2);
		      $sub .= " ...";
		    }

	    # ■コメント
		    $comment =~ s/<br>/ /gi;
		    
		    if (length ($comment) > $msg_len * 2) {
		      $comment = substr ($comment, 0, $msg_len * 2);
		      $comment .= " ...";
		    }

		# 自動リンク
			if ($autolink) { &auto_link($comment); }

			if ($tail) {
					$comment = "<a href=\"$imgurl$ftime$tail\" target=\"_blank\"><img src=\"$imgurl$IconClip\" border=0 alt='$ftime$tail'><b>$ftime$tail</b></a> " .$comment;
			}
			push @mesg, {'{$i}'=>$i,'{$newimg}'=>$newimg,'{$sub}'=>$sub,'{$name}'=>$name,'{$url}'=>$url,'{$date}'=>$date,'{$no}'=>$no,'{$reno}'=>$reno,'{$color}'=>$color,'{$comment}'=>$comment};
		    $i++;
		    if ($i > $look_cnt) { last; }
		}
		
		%args = (%args, ('{$mes}'=>\@mesg));
	}

  print "Content-type: text/html\n\n".$PTE->ParseBlock('chibi',\%args);

exit;
}
#----------------#
#  投稿記事受付  #
#----------------#
sub regist {
	local($top,$ango,$f,$match,$tail,$W,$H,@lines,@new,@tmp);

	# フォーム入力チェック
	&form_check;

	# 時間を取得
	&get_time;

	# クッキーを発行
	&set_cookie($in{'name'},$in{'email'},$in{'url'},$in{'pwd'},$in{'icon'},$in{'color'});

	# ファイルロック
	if ($lockkey) { &lock; }

	# ログを開く
	&loadlog($logfile) if (!defined $logs);

	# 記事NO処理
	my($no,$ip,$time2) = split(/<>/, $top);
	$no++;
	$top="$no<>$addr<>$times<>\n";

	# 連続投稿チェック
	if ($addr eq $ip && $wait > $times - $time2)
		{ &error($jnstr_contpost); }

	# トリップ生成
	if ($gentrip) { &tripping; }

	# IDを作成
	if($genid) { &makeid; $idpre=" ID:"; }
	else { $idcrypt = ""; $idpre="";}

	#fusianasan処理
	$in{'name'} =~ s%mokorikomo%<span class="nor">$addr</span>%;
	$in{'name'} =~ s%fusianasan%<span class="nor">$host</span>%;

	# sage機能
	if ($sagemode) { &sage; }

	# 削除キーを暗号化
	if ($in{'pwd'} ne "") { $ango = &encrypt($in{'pwd'}); }

	# ファイル添付処理
	if ($in{'upfile'}) { ($ftime,$tail,$W,$H) = &upload; }

	# 親記事の場合
	if ($in{'reno'} eq "") {
		$logs{$no}= [$no,"","$date$idpre$idcrypt",$in{'name'},$in{'email'},$in{'sub'},$in{'comment'},$in{'url'},$host,$ango,$in{'color'},$tail,$W,$H,0,$ftime,"\n" ];
		$trees{$no}= [$no];
		unshift(@torder,$no);

		# 更新
		&savelog($logfile);

		# 過去ログ更新
		if ($pdata[0]) { &pastlog; }

	# レス記事の場合
	} else {
		if (!exists($trees{$in{'reno'}})) { &error($jnstr_wrongreq); }
		$s_flag = 1 if($maxage && ($#{$trees{$in{'reno'}}} +1) > $maxage); # 強制sageスレ
		&bumppost(tSearch($in{'reno'})) if (($topsort||$a_flag) && !$s_flag); #トップソートあり

		$logs{$no}=[$no,$in{'reno'},"$date$idpre$idcrypt",$in{'name'},$in{'email'},$in{'sub'},$in{'comment'},$in{'url'},$host,$ango,$in{'color'},$tail,$W,$H,0,$ftime,"\n" ];
		push(@{$trees{$in{'reno'}}},$no);
		
		# 更新
		&savelog($logfile);
	}

	# ロック解除
	if ($lockkey) { &unlock; }

	if($storage_limit) {
		$currusage=totalsize(1);
		&detect_storage;
	}

	# メール処理
	if ($mailing == 1 && $in{'email'} ne $mailto) { &mail_to; }
	elsif ($mailing == 2) { &mail_to; }

	# リロード
	if ($location) {
		if ($ENV{'PERLXS'} eq "PerlIS") {
			print "HTTP/1.0 302 Temporary Redirection\r\n";
			print "Content-type: text/html\n";
		}
		print "Location: $location?\n\n";
	} else {
		&redirect($jnstr_posted,$redirection);
	}
	exit;
}

sub redirect {
		if ($htmlfile) { $outputhtml=1;&html_log;$outputhtml=0;}
		&header($_[1]);
		print "<div align=center><hr width=400>\n";
		print "<h3>$_[0]$jnstr_redirect</h3>\n";
		print "<form action=\"$dest\">\n";
		print "<input type=submit value='$jnstr_backtobbs'></form>\n";
		print "<hr width=400></div>\n</body></html>\n";
}

sub detect_storage{
	if(!$currusage){$total_usage=&totalsize;}
	else{$total_usage=$currusage;}
	
	if($total_usage<$storage_amount*1024){$limit=$storage_amount*1024;}
	else{$limit=$total_usage;}
	
	&getOversize;
	&auto_del_img if (keys(%deloversize));
}
sub totalsize{
	my($size);

	# ロック処理
	&lock if ($lockkey);

	# カウントファイルを読みこみ
	open(LOG,"+<$fscfile") || open(LOG,">$fscfile");
	eval "flock(LOG, 2);";
	$size = <LOG>;

	# 添付データサイズキャッシュアップ
	if ($_[0]) {
		&loadlog($logfile) if (!defined $logs);
		my $size=0;
		foreach (@porder) {
			($no,$reno,$date,$name,$mail,$sub,$com,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$_}};

			if ($tail && -e "${imgdir}$ftime$tail") {
				$size += -s "${imgdir}$ftime$tail";
				$size += -s "${imgdir}$ftime"."s.jpg" if(-e "${imgdir}$ftime"."s.jpg");
			}
		}

		truncate(LOG, 0);
		seek(LOG, 0, 0);
		print LOG "$size";
	}
	close(LOG);


	&unlock if ($lockkey);
	$size;
}
#--------------------#
#  画像アップロード  #
#--------------------#
sub upload {
	my($macbin,$fname,$flag,$upfile,$imgfile,$tail,$W,$W2,$H,$H2);

	# 画像処理
	$macbin=0;
	foreach (@in) {
		if (/(.*)Content-type:(.*)/i) { $tail=$2; }
		if (/(.*)filename=\"(.*)\"/i) { $fname=$2; }
		if (/application\/x-macbinary/i) { $macbin=1; }
	}
	$tail =~ s/\r//g;
	$tail =~ s/\n//g;

	# ファイル形式を認識
	$flag=0;
	if ($tail =~ /image\/gif/i && $gif) { $tail=".gif"; $flag=1; }
	if ($tail =~ /image\/p?jpeg/i && $jpeg) { $tail=".jpg"; $flag=1; }
	if ($tail =~ /image\/x-png/i && $png) { $tail=".png"; $flag=1; }
	if ($tail =~ /text\/plain/i && $text) { $tail=".txt"; $flag=1; }
	if ($tail =~ /lha/i && $lha) { $tail=".lzh"; $flag=1; }
	if ($tail =~ /zip/i && $zip) { $tail=".zip"; $flag=1; }
	if ($tail =~ /pdf/i && $pdf) { $tail=".pdf"; $flag=1; }
	if ($tail =~ /audio\/.*mid/i && $midi) { $tail=".mid"; $flag=1; }
	if ($tail =~ /msword/i && $word) { $tail=".doc"; $flag=1; }
	if ($tail =~ /ms-excel/i && $excel) { $tail=".xls"; $flag=1; }
	if ($tail =~ /ms-powerpoint/i && $ppt) { $tail=".ppt"; $flag=1; }
	if ($tail =~ /audio\/.*realaudio/i && $ram) { $tail=".ram"; $flag=1; }
	if ($tail =~ /application\/.*realmedia/i && $rm) { $tail=".rm"; $flag=1; }
	if ($tail =~ /video\/.*mpeg/i && $mpeg) { $tail=".mpg"; $flag=1; }
	if ($tail =~ /audio\/.*mpeg/i && $mp3) { $tail=".mp3"; $flag=1; }

	if (!$flag) {
		if ($fname =~ /\.gif$/i && $gif) { $tail=".gif"; $flag=1; }
		if ($fname =~ /\.jpe?g$/i && $jpeg) { $tail=".jpg"; $flag=1; }
		if ($fname =~ /\.png$/i && $png) { $tail=".png"; $flag=1; }
		if ($fname =~ /\.lzh$/i && $lha) { $tail=".lzh"; $flag=1; }
		if ($fname =~ /\.txt$/i && $text) { $tail=".txt"; $flag=1; }
		if ($fname =~ /\.zip$/i && $zip) { $tail=".zip"; $flag=1; }
		if ($fname =~ /\.pdf$/i && $pdf) { $tail=".pdf"; $flag=1; }
		if ($fname =~ /\.mid$/i && $midi) { $tail=".mid"; $flag=1; }
		if ($fname =~ /\.doc$/i && $word) { $tail=".doc"; $flag=1; }
		if ($fname =~ /\.xls$/i && $excel) { $tail=".xls"; $flag=1; }
		if ($fname =~ /\.ppt$/i && $ppt) { $tail=".ppt"; $flag=1; }
		if ($fname =~ /\.ram$/i && $ram) { $tail=".ram"; $flag=1; }
		if ($fname =~ /\.rm$/i && $rm) { $tail=".rm"; $flag=1; }
		if ($fname =~ /\.mpe?g$/i && $mpeg) { $tail=".mpg"; $flag=1; }
		if ($fname =~ /\.mp3$/i && $mp3) { $tail=".mp3"; $flag=1; }
	}

	# アップロード失敗処理
	if (!$flag || !$fname) {
		if (!$clip_err) { return; }
		else { &error($jnstr_cannotupload); }
	}

	$upfile = $in{'upfile'};

	# マックバイナリ対策
	if ($macbin) {
		$length = substr($upfile,83,4);
		$length = unpack("%N",$length);
		$upfile = substr($upfile,128,$length);
	}

	$filetime="";
	# 添付データを書き込み
	$filetime=&get_time(1);
	$imgfile = "$imgdir$filetime$tail";
	open(OUT,">$imgfile") || &error("アップロード失敗");
	binmode(OUT);
	binmode(STDOUT);
	print OUT $upfile;
	close(OUT);

	chmod (0666, $imgfile);

	# 画像サイズ取得
	if ($tail eq ".jpg") { ($W, $H) = &JpegSize($imgfile); }
	elsif ($tail eq ".gif") { ($W, $H) = &GifSize($imgfile); }
	elsif ($tail eq ".png") { ($W, $H) = &PngSize($imgfile); }

	# 画像表示縮小
	if ($W > $MaxW || $H > $MaxH) {
		$W2 = $MaxW / $W;
		$H2 = $MaxH / $H;
		if ($W2 < $H2) { $key = $W2; }
		else { $key = $H2; }
		$W = int ($W * $key) || 1;
		$H = int ($H * $key) || 1;
		eval{ imgbbs::imgresize("$imgfile","${imgdir}$filetime"."s.jpg",$W,$H,$QL,1); };
	}

	return ($filetime,$tail,$W,$H);
}

sub th_table {
	if ($_[0] eq 1) {$th_log=99999;	&header(); &showtitle; &showmenu("[<a href=\"$dest\">$jnstr_backtobbs</a>]");}
	$th_no = 1;
	$res_cnt = 0;
	if ($th_log) {
		if (!$gazouliked) {
			if ($_[0] eq 1) {print "<center>";}
			print "<table border=1 width=90% cellspacing=0 cellpadding=2 bgcolor=\"$tbl_color\"><tr><td>\n";
		}else {
			if ($_[0] ne 1) {print "<hr>";}
			print "<table border=0 width=100%><tr><td>\n";
		}
	}
	&loadlog($logfile) if (!defined $logs);
	for (my $i=0;$i<=$#torder;$i++) {
		$res_cnt=$#{$trees{$torder[$i]}};
		($no,$reno,$date,$name,$mail,$sub,$com,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$torder[$i]}};
		if ($th_log) {
			if ($th_no < $th_log + 1) {
				if ($sub_len && length($sub) > $sub_len*2) {
					$sub = substr($sub,0,$sub_len*2);
					$sub .= "...";
				}
				print "$th_no:<a href=\"$script?mode=res&amp;no=$no&amp;$langstr\">$sub</a>";
				print "[$res_cnt] \n";
				if ($_[0] eq 1) {print "<br>";}
				$th_no++;
			}
		}
		push (@res_log,$res_cnt);
	}

	if ($th_log) {
		if ($_[0] ne 1) {print "<div align=\"right\"><b><a href=\"$script?mode=threads&amp;$langstr\">$jnstr_allthread</a></b></div>\n";}
		print "</td></tr></table>\n";
		if (!$gazouliked) {
			print "<br><br>";
		}
	}
	if ($_[0] eq 1) {
		if (!$gazouliked) {print "</center>\n";}
		&footer;
		exit;
	}
}

#----------------#
#  返信フォーム  #
#----------------#
sub res_form {
	my($no,$reno,$date,$name,$mail,$sub,$com,$url);

	# ヘッダを出力
	&header;

	&getOversize;

	# ログを読み込み
	&loadlog($logfile) if (!defined $logs);

	if (!exists($trees{$in{'no'}})) { &error($jnstr_wrongreq); }

	# 関連記事出力
	&showtitle;
	&showmenu("[<a href=\"javascript:history.back()\">$jnstr_back</a>] [<a href=\"#RES\">$jnstr_gotores</a>]");
	$flag=0;
	my %args=();
	foreach (@{$trees{$in{'no'}}}) {
		($no,$reno,$date,$name,$mail,$sub,$com,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$_}};

		if ($in{'no'} == $no) { $resub = $sub; }
		if ($url) { $url = "&lt;<a href=\"http://$url\">HOME</a>&gt;"; }
		$com =~ s/([>]|^)(&gt;[^<]*)/$1<span class='quoted'>$2<\/span>/g;
		if ($mail) { $name = "<a href=\"mailto:$mail\">$name</a>"; }

		$imgsrc=attechmentProc($tail,$w,$h);
		# 自動リンク
		if ($autolink) { &auto_link($com); }


		if(!$reno) { # First post = thread top
			my $bekill;
			$bekill=$oversize{$no}?"<span class='warn_txt2'>$jnstr_size_warn</span><br/>\n":'';
			%args=('{$NAME_TEXT}'=>"$jnstr_poster$jnstr_colon",'{$TIME_TEXT}'=>"$jnstr_postdate$jnstr_colon",'{$NO}'=>$no,
				'{$ATTECH}'=>$attech,'{$SUB}'=>$sub,'{$NAME}'=>$name,'{$TIME}'=>$date,'{$REPLYBTN}'=>"",'{$resmode}'=>1,
				'{$PROCBTN}'=>"<a href=\"#proc\" OnClick=\"document.Proc.no.value='$no';\">$jnstr_proclink</a>\n",'{$COL}'=>$color,'{$COM}'=>$com,'{$WARN_BEKILL}'=>$bekill,'{$WARN_HIDEPOST}'=>'','{$res}'=>());
		} else {
			$bekill=$oversize{$no}?"<span class='warn_txt2'>$jnstr_size_warn</span><br/>\n":'';
			push @{$args{'{$res}'}},{'{$NAME_TEXT}'=>"$jnstr_poster$jnstr_colon",'{$TIME_TEXT}'=>"$jnstr_postdate$jnstr_colon",'{$NO}'=>$no,
				'{$ATTECH}'=>$attech,'{$SUB}'=>$sub,'{$NAME}'=>$name,'{$TIME}'=>$date,'{$PROCBTN}'=>"<a href=\"#proc\" OnClick=\"document.Proc.no.value='$no';\">$jnstr_proclink</a>\n",
				'{$COL}'=>$color,'{$COM}'=>$com,'{$WARN_BEKILL}'=>$bekill,'{$resmode}'=>1};
		}
	}

	# タイトル名
	if ($resub !~ /^Re\:/) { $resub = "Re\: $resub"; }

#	print &form("res","","","","","",$resub,"","","","","");

#	print $PTE->ParseBlock('thread',\%args);
	print $PTE->ParseBlock('resform',{'{$THREAD}'=>$PTE->ParseBlock('thread',\%args),'{$FORM}'=>"<a name=\"RES\"></a>".&form("res","","","","","",$resub,"","","","","")});

	&procform($in{'no'});

	&footer;
	exit;
}

#--------------------#
#  添付ファイル処理  #
#--------------------#
sub attechmentProc {
	my($tail,$w,$h)=@_;
	my $imgsrc;
	if ($tail eq ".gif" || $tail eq ".jpg" || $tail eq ".png") {
		if ($ImageCheck && $chk != 1) {
			$imgsrc="<img src=\"$imgurl$IconSoon\">\n";
		} else {
			if ($w && $h) { $wh="width=$w height=$h"; }
			else { $wh=""; }
			my $imgWH='';
			if ($w && $h) { $wh="width=$w height=$h"; }
			else { $wh=""; }
			if ($ShowWH && -e "$imgdir$ftime$tail") {
				if ($tail eq ".gif") {
					($iW,$iH) = &GifSize("$imgdir$ftime$tail");
				}elsif ($tail eq ".jpg") {
					($iW,$iH) = &JpegSize("$imgdir$ftime$tail");
				}elsif ($tail eq ".png") {
					($iW,$iH) = &PngSize("$imgdir$ftime$tail");
				}
				$imgWH=" ,".$iW."x".$iH;
			}
			$fsize=-s "${imgdir}$ftime$tail";
			$fsize=sprintf("%.2f",$fsize/1024);
			if (-e "$imgdir${ftime}s.jpg"){
				$imgsrc="<a href=\"$imgurl$ftime$tail\" target='_blank'><small>$jnstr_img_pre$ftime$tail ($fsize KB$imgWH) $jnstr_img_sample</small><br><img src=\"${imgurl}$ftime"."s.jpg\" border=0 $ipt $wh alt=\"$ftime$tail ($fsize KB$imgWH)\"></a>\n";
			} elsif (-e "$imgdir$ftime$tail") {
				$imgsrc="<a href=\"$imgurl$ftime$tail\" target='_blank'><small>$jnstr_img_pre$ftime$tail ($fsize KB$imgWH)</small><br><img src=\"$imgurl$ftime$tail\" border=0 $ipt $wh alt=\"$ftime$tail ($fsize KB$imgWH)\"></a>\n";
			} else {
				$imgsrc="<a href=\"$imgurl$ftime$tail\" target='_blank'><small>$jnstr_img_pre$ftime$tail</small></a>\n";
			}
		}
	} elsif ($tail) {
		$imgsrc="<a href=\"$imgurl$ftime$tail\"><img src=\"$imgurl$IconClip\" border=0 alt='Download:$ftime$tail'></a> <b>$ftime$tail</b>\n";
	} else {
		$imgsrc="";
	}
	return $imgsrc;
}

#----------------#
#  デコード処理  #
#----------------#
sub decode {
	my($key,$val);
	undef(%in);

	$cgi_lib'maxdata = $maxsize;

	&ReadParse;
	while ( ($key,$val) = each(%in) ) {

		next if ($key eq "upfile");

		# シフトJISコード変換
#		&jcode'convert(*val, "sjis", "", "z");

		# タグ処理
		$val =~ s/&/&amp;/g;
		$val =~ s/&amp;#/&#/g;
		$val =~ s/"/&quot;/g;
		$val =~ s/</&lt;/g;
		$val =~ s/>/&gt;/g;

		# 改行処理
		$val =~ s/\r\n/<br>/g;
		$val =~ s/\r/<br>/g;
		$val =~ s/\n/<br>/g;

		$in{$key} = $val;
	}
	$mode = $in{'mode'};
	$page = $in{'page'} if defined $in{'page'};
}

#------------#
#  留意事項  #
#------------#
sub howto {
	if ($in_email) {
		$eml_msg = $jnstr_howto_email1;
	} else {
		$eml_msg = $jnstr_howto_email2;
	}

	$maxkb = int ($cgi_lib'maxdata / 1024);
	if ($gif) { $FILE .= "GIF, "; }
	if ($jpeg) { $FILE .= "JPEG, "; }
	if ($png) { $FILE .= "PNG, "; }
	if ($text) { $FILE .= "TEXT, "; }
	if ($lha) { $FILE .= "LHA, "; }
	if ($zip) { $FILE .= "ZIP, "; }
	if ($pdf) { $FILE .= "PDF, "; }
	if ($midi) { $FILE .= "MIDI, "; }
	if ($word) { $FILE .= "WORD, "; }
	if ($excel) { $FILE .= "EXCEL, "; }
	if ($ppt) { $FILE .= "POWERPOINT, "; }
	if ($rm) { $FILE .= "RM, "; }
	if ($ram) { $FILE .= "RAM, "; }
	if ($mpeg) { $FILE .= "MPEG, "; }
	if ($mp3) { $FILE .= "MP3, "; }
	$FILE =~ s/\, $//;

	&header;
	print <<"HTML";
[<a href="$dest">$jnstr_backtobbs</a>]
<table width="100%">
<tr><th bgcolor="#880000">
  <font color="#FFFFFF">$jnstr_howto_title</font>
</th></tr></table>
<p><center>
<table width="90%" border=1 cellpadding=10>
<tr><td bgcolor="$tbl_color">
<OL style="line-height:2.5em">
<li>$jnstr_howto_cookie</li>
<li>$jnstr_howto_upload_pre</li>
</ol><blockquote><ul style="line-height:2.5em">
  <li>$jnstr_howto_upload_type $FILE</li>
  <li>$jnstr_howto_upload_maxsize $maxkb KB</li>
  <li>$jnstr_howto_upload_img_pre $MaxW $jnstr_howto_upload_img_mid $MaxH $jnstr_howto_upload_img_post</li>
</ul></blockquote><ol style="line-height:2.5em">
<li value=3>$jnstr_howto_notag</li>
<li>$eml_msg</li>
<li>$jnstr_howto_logmax_pre $max$jnstr_howto_logmax_post</li>
<li>$jnstr_howto_pastlog_pre$script?mode=find&amp;$langstr$jnstr_howto_pastlog_post</li>
$jnstr_howto_otherrules
</OL>
</td></tr></table>
</center>
</body>
</html>
HTML
	exit;
}

#------------------#
#  ワード検索処理  #
#------------------#
sub find {
	&header;
	&showtitle;
	&showmenu("[<a href=\"$dest\">$jnstr_backtobbs</a>]");
	print <<"EOM";
<br>
<ul>
$jnstr_find_pre
</ul><dl><dd><form action="$script?$langstr" method="POST">
<input type=hidden name=mode value="find">
$jnstr_find_keyword<input type=text name=word size=30 value="$in{'word'}">
$jnstr_cond<select name=cond>
EOM
	if (!$in{'cond'}) { $in{'cond'} = "AND"; }
	foreach ("AND", "OR") {
		if ($in{'cond'} eq "$_") {
			print "<option value=\"$_\" selected>$_\n";
		} else {
			print "<option value=\"$_\">$_\n";
		}
	}
	print "</select>\n";
	print "$jnstr_display<select name=view>\n";
	if ($in{'view'} eq "") { $in{'view'} = $p_log; }
	foreach (5,10,15,20) {
		if ($in{'view'} == $_) {
			print "<option value=\"$_\" selected>$_$jnstr_post_n\n";
		} else {
			print "<option value=\"$_\">$_$jnstr_post_n\n";
		}
	}
	print "</select>\n";
	print "<input type=submit value='$jnstr_search'></form></dd>\n</dl>\n";

	# ワード検索の実行と結果表示
	if ($in{'word'} ne "") {

		# 入力内容を整理
		$in{'word'} =~ s/　/ /g;
		@pairs = split(/\s+/, $in{'word'});

		# ファイルを読み込み
		@new=();
		open(IN,"$logfile") || &error("Open Error : $logfile");
		$top = <IN>;
		while (<IN>) {
			$flag=0;
			foreach $pair (@pairs) {
				if (index($_,$pair) >= 0) {
					$flag=1;
					if ($in{'cond'} eq 'OR') { last; }
				} else {
					if ($in{'cond'} eq 'AND') { $flag=0; last; }
				}
			}
			if ($flag) { push(@new,$_); }
		}
		close(IN);

		# 検索終了
		$count = @new;
		print "$jnstr_search_result<b>$count</b>$jnstr_post_n\n";
		if ($page eq '') { $page = 0; }
		$end_data = @new - 1;
		$page_end = $page + $in{'view'} - 1;
		if ($page_end >= $end_data) { $page_end = $end_data; }

		$next_line = $page_end + 1;
		$back_line = $page - $in{'view'};

		$enwd = &url_enc($in{'word'});
		if ($back_line >= 0) {
			print "[<a href=\"$script?mode=find&amp;page=$back_line&amp;word=$enwd&amp;view=$in{'view'}&amp;cond=$in{'cond'}&amp;$langstr\">$jnstr_prevpage_pre$in{'view'}$jnstr_post_n</a>]\n";
		}
		if ($page_end ne "$end_data") {
			print "[<a href=\"$script?mode=find&amp;page=$next_line&amp;word=$enwd&amp;view=$in{'view'}&amp;cond=$in{'cond'}&amp;$langstr\">$jnstr_nextpage_pre$in{'view'}$jnstr_post_n</a>]\n";
		}
		print "[<a href=\"$script?mode=find&amp;$langstr\">$jnstr_search_again</a>]\n";

		my %args=('{$p}'=>());
		foreach ($page .. $page_end) {
			($no,$reno,$date,$name,$mail,$sub,$com,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime)
						= split(/<>/, $new[$_]);

			if ($url) { $url = "&lt;<a href=\"http://$url\">HOME</a>&gt;"; }
			$com =~ s/([>]|^)(&gt;[^<]*)/$1<span class='quoted'>$2<\/span>/g;
			if ($mail) { $name = "<a href=\"mailto:$mail\">$name</a>"; }

			$imgsrc=attechmentProc($tail,$w,$h);
			# 自動リンク
			if ($autolink) { &auto_link($com); }

			my $bekill;
			$bekill=$oversize{$no}?"<span class='warn_txt2'>$jnstr_size_warn</span><br/>\n":'';
			push @{$args{'{$p}'}},{'{$NAME_TEXT}'=>"$jnstr_poster$jnstr_colon",'{$TIME_TEXT}'=>"$jnstr_postdate$jnstr_colon",'{$NO}'=>$no,'{$RENO}'=>($reno)?"(Re:$reno)":'',
				'{$ATTECH}'=>$attech,'{$SUB}'=>$sub,'{$NAME}'=>$name,'{$TIME}'=>$date,'{$PROCBTN}'=>"<a href=\"#proc\" OnClick=\"document.Proc.no.value='$no';\">$jnstr_proclink</a>\n",
				'{$COL}'=>$color,'{$COM}'=>$com,'{$WARN_BEKILL}'=>$bekill};

		}
		print $PTE->ParseBlock('searchresults',\%args);
		&procform();
	}
	
	&footer;
	exit;
}

#----------------#
#  クッキー発行  #
#----------------#
sub set_cookie {
	my(@cook) = @_;
	my($gmt, $cook, @t, @m, @w);

	@t = gmtime(time + 60*24*60*60);
	@m = ('Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec');
	@w = ('Sun','Mon','Tue','Wed','Thu','Fri','Sat');

	# 国際標準時を定義
	$gmt = sprintf("%s, %02d-%s-%04d %02d:%02d:%02d GMT",
			$w[$t[6]], $t[3], $m[$t[4]], $t[5]+1900, $t[2], $t[1], $t[0]);

	# 保存データをURLエンコード
	foreach (@cook) {
		s/(\W)/sprintf("%%%02X", unpack("C", $1))/eg;
		$cook .= "$_<>";
	}

	# 格納
	print "Set-Cookie: JoyfulNote=$cook; expires=$gmt\n";
}

#----------------#
#  クッキー取得  #
#----------------#
sub get_cookie {
	my($key, $val); local(*cook);

	# クッキーを取得
	$cook = $ENV{'HTTP_COOKIE'};

	# 該当IDを取り出す
	foreach ( split(/;/, $cook) ) {
		($key, $val) = split(/=/);
		$key =~ s/\s//g;
		$cook{$key} = $val;
	}

	# データをURLデコードして復元
	foreach ( split(/<>/, $cook{'JoyfulNote'}) ) {
		s/%([0-9A-Fa-f][0-9A-Fa-f])/pack("C", hex($1))/eg;

		push(@cook,$_);
	}
	return (@cook);
}

#--------------#
#  エラー処理  #
#--------------#
sub error {
	&unlock if ($lockflag);
	&header if (!$headflag);

	print "<center><hr width=400><h3>ERROR !</h3>\n";
	print "<font color=red>$_[0]</font>\n";
	print "<p><hr width=400></center>\n";
	print "</body></html>\n";
	exit;
}

#--------------#
#  管理モード  #
#--------------#
sub admin {
	if ($in{'pass'} eq "") {
		&header;
		print "[<a href=\"$dest\">$jnstr_backtobbs</a>]";
		print "[<a href=\"$script?mode=remake&amp;$langstr\">$jnstr_admin_remake</a>]" if($htmlfile);
		print "[<a href=\"mod_cap.cgi?$langstr\">$jnstr_admin_cap</a>]";
		print "<center><h4>$jnstr_admin_pre</h4>\n";
		print "<form action=\"$script?$langstr\" method=\"POST\">\n";
		print "<input type=hidden name=mode value=\"admin\">\n";
		print "<input type=hidden name=action value=\"del\">\n";
		print "<input type=password name=pass size=8>\n";
		print "<input type=submit value=\" $jnstr_verify \"></form>\n";
		print "</center>\n</body></html>\n";
		exit;
	}
	if ($in{'pass'} ne $pass) { &error($jnstr_wrongpass); }

	&loadlog($logfile) if (!defined $logs);

	&header;
	print "[<a href=\"$dest\">$jnstr_backtobbs</a>]\n";
	print "<table width='100%'><tr><th bgcolor=\"#804040\">\n";
	print "<font color=\"#FFFFFF\">$jnstr_adminmode</font>\n";
	print "</th></tr></table>\n";

	# 画像許可
	if ($in{'chk'}) {
		@CHK = split(/\0/, $in{'chk'});

		# ロック処理
		if ($lockkey) { &lock; }

		# 許可情報をマッチングし更新
		foreach (@CHK) {
			$logs{$_}[14]=1;
		}

		# 更新
		&savelog($logfile);

		# ロック解除
		if ($lockkey) { &unlock; }
		if ($htmlfile) { $outputhtml=1;&html_log;$outputhtml=0;}
	}
	# 削除処理
	if ($in{'del'}) {
		@DEL = split(/\0/, $in{'del'});

		# ロック処理
		if ($lockkey) { &lock; }

		# 削除情報をマッチングし更新
		&removepost(@DEL);

		# 更新
		&savelog($logfile);

		# ロック解除
		if ($lockkey) { &unlock; }
		if ($htmlfile) { $outputhtml=1;&html_log;$outputhtml=0;}
	}

	# 管理を表示
	if ($page eq "") { $page = 0; }
	print "<p><center><table><tr><td>\n<ul>\n";
	print $jnstr_adminpage_pre;
	print "</ul>\n</td></tr></table>\n";
	print "<form action=\"$script?$langstr\" method=\"POST\">\n";
	print "<input type=hidden name=mode value=\"admin\">\n";
	print "<input type=hidden name=page value=\"$page\">\n";
	print "<input type=hidden name=pass value=\"$in{'pass'}\">\n";
	print "<input type=hidden name=action value=\"$in{'action'}\">\n";
	print "<input type=submit value=\"$jnstr_submit\">";
	print "<input type=reset value=\"$jnstr_reset\">\n";
	print "<p><table border=0 cellspacing=1><tr>\n";
	print "<th nowrap>$jnstr_usr_del</th><th nowrap>$jnstr_resno</th><th>$jnstr_postdate</th>";
	print "<th>$jnstr_title</th><th>$jnstr_poster</th><th>URL</th><th>$jnstr_commets</th>";
	print "<th>$jnstr_hostname</th><th>$jnstr_img<br>(bytes)</th>\n";

	$line=9;
	if ($ImageCheck) { print "<th>$jnstr_allowimg</th>"; $line=10; }

	print "</tr>\n";

	for (my $i=0;$i<=$#torder;$i++) {
		for (my $j=$#{$trees{$torder[$i]}};$j>=0;$j--) {
			$img_flag=0;
			($no,$reno,$date,$name,$mail,$sub,$com,$url,$host,$pw,$color,$tail,$w,$h,$chk,$ftime) = @{$logs{$trees{$torder[$i]}[$j]}};

			if ($mail) { $name="<a href=\"mailto:$mail\">$name</a>"; }
			($date) = split(/\(/, $date);

			if ($url) { $url = "&lt;<a href=\"http://$url\" target='_top'>Home</a>&gt;"; }
			else { $url = '-'; }

			$com =~ s/<br>//ig;
			$com =~ s/</&lt;/g;
			$com =~ s/>/&gt;/g;
			if (length($com) > 40) {
				$com = substr($com,0,38);
				$com .= "...";
			}
			if ($tail && -e "$imgdir$ftime$tail") {
				if ($tail eq ".gif" || $tail eq ".jpg" || $tail eq ".png") {
					$img_flag = 1;
					$File = $jnstr_img;
				} else { $File = "File"; }
				$clip = "<a href=\"$imgurl$ftime$tail\" target='_blank'>$File</a>";
				$size = -s "$imgdir$ftime$tail";
				$all += $size;
			} else {
				$clip = "";
				$size = 0;
			}

			# チェックボックス
			print "<tr><th><input type=checkbox name=del value=\"$no\"></th>";
			print "<td align=center>$no</td>";
			print "<td>$date</td><th>$sub</th><th>$name</th>";
			print "<td align=center>$url</td><td>$com</td>";
			print "<td>$host</td><td align=center>$clip<br>($size)</td>\n";
			# 画像許可
			if ($ImageCheck) {
				if ($img_flag == 1 && $chk == 1) {
					print "<th>OK</th>";
				} elsif ($img_flag == 1 && $chk != 1) {
					print "<th><input type=checkbox name=chk value=$no></th>";
				} else {
					print "<td><br></td>";
				}
			}
			print "</tr>\n";
		}
		print "<tr><th colspan=$line><hr></th></tr>\n";
	}
	print "</table></form>\n";

	$all = int ($all / 1024);
	print "$jnstr_attachfile_total_pre <b>$all</b> $jnstr_attachfile_total_post\n";
	print "</center>\n";
	print "</body></html>\n";
	exit;
}

#------------------------#
#  添付ファイル削除処理  #
#------------------------#
sub auto_del_img{
	my @posts=();
	# ロック処理
	&lock if ($lockkey);

	&loadlog($logfile) if (!defined $logs);

	foreach ( keys(%deloversize) ) {
		push(@posts,$_);
	}

	&removeattachment(@posts);

	&savelog($logfile);

	&unlock if ($lockkey);

	$currusage=totalsize(1);

}

#------------------#
#  ユーザ記事削除  #
#------------------#
sub usr_del {
	if ($in{'no'} eq '' || ($in{'pwd'} eq '' && !$modnopass))
		{ &error($jnstr_user_mod_err); }
	$hosti=$host if($modnopass);

	# ロック処理
	if ($lockkey) { &lock; }

	&loadlog($logfile) if (!defined $logs);

	if (!exists($logs{$in{'no'}})) { &error($jnstr_postnotfound); }

	$pw2 = $logs{$in{'no'}}[9];

	if (!$modnopass) {
		if ($pw2 eq "" && $in{'pwd'} ne "$pass") { &error($jnstr_user_mod_nopass); }
	}
	if ($modnopass && !$pw2) {
		if ($host2 ne "$hosti" && $in{'pwd'} ne "$pass") { &error($jnstr_user_mod_nopass); }
	}
	if ($pw2) {
		if( $in{'pwd'} eq "$pass"){$check="yes";}
		else{$check = &decrypt("$in{'pwd'}","$pw2");}
		if ($check ne "yes") { &error($jnstr_user_mod_wrongpass); }
	}

	if ($mode eq "usr_del_img") {
		&removeattachment($in{'no'});
	} else {
		&removepost($in{'no'});
	}

	# 更新
	&savelog($logfile);


	# ロック解除
	if ($lockkey) { &unlock; }

	if($storage_limit) {
		$currusage=totalsize(1);
		&detect_storage;
	}

	&redirect($mode eq "usr_del_img"?$jnstr_user_del_img:$jnstr_user_del,$redirection);
	exit;
}

#----------------#
#  記事修正処理  #
#----------------#
sub usr_edt {
	if ($in{'no'} eq '' || ($in{'pwd'} eq '' && !$modnopass))
		{ &error($jnstr_user_mod_err); }
	$hosti=$host if($modnopass);

	&loadlog($logfile) if (!defined $logs);

	if (!exists($logs{$in{'no'}})) { &error($jnstr_postnotfound); }

	($host2, $pw2) = @{$logs{$in{'no'}}}[8, 9];

	if (!$modnopass) {
		if ($pw2 eq "" && $in{'pwd'} ne "$pass") { &error($jnstr_user_mod_nopass); }
	}
	if ($modnopass && !$pw2) {
		if ($host2 ne "$hosti" && $in{'pwd'} ne "$pass") { &error($jnstr_user_mod_nopass); }
	}
	if ($pw2) {
		if( $in{'pwd'} eq "$pass"){$check="yes";}
		else{$check = &decrypt("$in{'pwd'}","$pw2");}
		if ($check ne "yes") { &error($jnstr_user_mod_wrongpass); }
	}

	if ($in{'action'} eq "edit") {

		# フォーム入力チェック
		&form_check;

		# ロック処理
		&lock if ($lockkey);

		if ($gentrip) { &tripping; }
		@{$logs{$in{'no'}}}[3, 4, 5, 6, 7, 10] = ($in{'name'}, $in{'email'}, $in{'sub'}, $in{'comment'}, $in{'url'}, $in{'color'});
		&savelog($logfile);

		&unlock if ($lockkey);

		if ($in{'url'}) { $in{'url'} = "<a href=\"http://$in{'url'}\" target=\"_top\">http://$in{'url'}</a>"; }
		if ($in{'email'}) { $in{'email'} = "<a href=\"mailto:$in{'email'}\">$in{'email'}</a>"; }

		if ($htmlfile) { $outputhtml=1;&html_log;$outputhtml=0;}
		&header($redirection);
		&showtitle;
		&showmenu("<h4>- $jnstr_user_edted_pre -</h4>");
		print "<center><table>\n";
		print "<tr><td>$jnstr_name</td><td>： <b>$in{'name'}</b></td></tr>\n";
		print "<tr><td>$jnstr_email</td><td>： $in{'email'}</td></tr>\n";
		print "<tr><td>$jnstr_title</td><td>： <b>$in{'sub'}</b></td></tr>\n";
		print "<tr><td>URL</td><td>： $in{'url'}</td></tr>\n";
		print "<tr><td valign=top>$jnstr_commets</td><td>：<font color=\"$in{'color'}\">$in{'comment'}</font>\n";
		print "</td></tr></table>\n";
		print "<form action=\"$dest\">\n";
		print "<input type=submit value=' $jnstr_backtobbs '></form></center>\n";
		&footer;
		exit;
	}

	$logs{$in{'no'}}[6] =~ s/<br>/\r/g;
	$logs{$in{'no'}}[3] =~ s/<font color.*?>//g;
	$logs{$in{'no'}}[3] =~ s/<span class="nor">//g;
	$logs{$in{'no'}}[3] =~ s/<\/font>//g;
	$logs{$in{'no'}}[3] =~ s/<\/span>//g;
	$logs{$in{'no'}}[3] =~ s/★.*//g;
	$logs{$in{'no'}}[3] =~ s/◆.*//g;

	$logs{$in{'no'}}[9] = $in{'pwd'};

	&header;
	&showtitle;
	&showmenu("[<a href=\"javascript:history.back()\">$jnstr_back</a>]");
	print "<p>- $jnstr_user_edt_pre -<br>\n";
	print &form("edit",@{$logs{$in{'no'}}});
	&footer;
	exit;
}

#------------------------#
#  フォーム入力チェック  #
#------------------------#
sub form_check {
	# 他サイトからのアクセスを排除
	if ($base_url) {
		$baseUrl =~ s/(\W)/\\$1/g;

		$ref = $ENV{'HTTP_REFERER'};
		$ref =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
		if ($ref && $ref !~ /$base_url/i) { &error($jnstr_wrongaddr); }
	}

	# methodプロパティはPOST限定
	if ($ENV{'REQUEST_METHOD'} ne 'POST') { &error($jnstr_wrongpost); }

	# check field traps
	foreach $key (@traps) {
		if($in{$key} ne "") { &error($jnstr_wrongpost) };
	}

	# replace fields
	%replace = reverse %fields;
	foreach $key (keys %replace) {
		$in{$replace{$key}} = $in{$key};
	}

	$in{'url'} =~ s/^http\:\/\///;
	# 入力項目のチェック
	$in{'name'}=~s/\0+//g;
	$in{'email'}=~s/\0+//g;
	$in{'sub'}=~s/\0+//g;
	$in{'comment'}=~s/\0+//g;
	if ($in{'name'} eq "") {if (!$anonymouspost) {&error($jnstr_namemissed);}else{$in{'name'}=$jnstr_noname;} }
	if ($in{'sub'} eq "") { $in{'sub'} = $jnstr_notitle; }
	if ($in{'comment'} eq "") {if(!$in{'upfile'}) {&error($jnstr_commentsmissed);}else{$in{'comment'}=$jnstr_nocomments;} }
	if ($in_email) {
		if ($in{'email'} eq "") { &error($jnstr_emailmissed); }
		elsif ($in{'email'} !~ /[\w\.\-]+\@[\w\.\-]+\.[a-zA-Z]{2,6}$/) {
			&error($jnstr_wrongemail);
		}
	}
	if ($bpngckey && $in{'email'} =~ /\!$bpngckey/) {
		$bypressNGC=1;
		$in{'email'} =~ s/\!$bpngckey//g;
	}
	if($NGWords && !$bypressNGC) { foreach($in{'name'},$in{'email'},$in{'url'},$in{'sub'},$in{'comment'}) {&NGcheck($_);} }
}

#--------------#
#  時間を取得  #
#--------------#
sub get_time {
	my($ft);
	$ENV{'TZ'} = $TZ;
	$times = time;
	($sec,$min,$hour,$mday,$mon,$year,$wday) = (localtime($times))[0..6];
	@week = ('Sun','Mon','Tue','Wed','Thu','Fri','Sat');

	# 日時のフォーマット
	$date = sprintf("%04d/%02d/%02d(%s) %02d:%02d",
			$year+1900,$mon+1,$mday,$week[$wday],$hour,$min);
	if ($_[0] eq 1){
		$ft = sprintf("%04d%02d%02d%02d%02d%02d",$year+1900,$mon+1,$mday,$hour,$min,$sec);
		$ft;
	}
}

#----------------#
#  カウンタ処理  #
#----------------#
sub counter {
	my($count, $cntup, @count);

	# 閲覧時のみカウントアップ
	if ($mode eq '') { $cntup = 1; } else { $cntup = 0; }

	# カウントファイルを読みこみ
	open(LOG,"+< $cntfile") || &error("Open Error: $cntfile");
	eval "flock(LOG, 2);";
	$count = <LOG>;

	# IPチェックとログ破損チェック
	my($cnt, $ip) = split(/:/, $count);
	if ($addr eq $ip || $cnt eq "") { $cntup = 0; }

	# カウントアップ
	if ($cntup) {
		$cnt++;
		truncate(LOG, 0);
		seek(LOG, 0, 0);
		print LOG "$cnt:$addr";
	}
	close(LOG);

	# 桁数調整
	while(length($cnt) < $mini_fig) { $cnt = '0' . $cnt; }
	@cnts = split(//, $cnt);

	# GIFカウンタ表示
	if ($counter == 2) {
		foreach (0 .. $#cnts) {
			print "<img src=\"$gif_path$cnts[$_]\.gif\" alt=\"$cnts[$_]\" width=\"$mini_w\" height=\"$mini_h\">";
		}
	}
	# テキストカウンタ表示
	else {
		print "<font color=\"$cnt_color\" face=\"verdana,Times New Roman,Arial\">$cnt</font><br>\n";
	}
}

#--------------#
#  ロック処理  #
#--------------#
sub lock {
	# 古いロックは削除する
	if (-e $lockfile) {
		my($mtime) = (stat($lockfile))[9];
		if ($mtime < time - 30) { &unlock; }
	}

	my($retry) = 5;

	# symlink関数式ロック
	if ($lockkey == 1) {
		while (!symlink(".", $lockfile)) {
			if (--$retry <= 0) { &error('LOCK is BUSY'); }
			sleep(1);
		}

	# mkdir関数式ロック
	} elsif ($lockkey == 2) {
		while (!mkdir($lockfile, 0755)) {
			if (--$retry <= 0) { &error('LOCK is BUSY'); }
			sleep(1);
		}
	}
	$lockflag=1;
}

#--------------#
#  ロック解除  #
#--------------#
sub unlock {
	if ($lockkey == 1) { unlink($lockfile); }
	elsif ($lockkey == 2) { rmdir($lockfile); }
	$lockflag=0;
}

#--------------#
#  メール送信  #
#--------------#
sub mail_to {
	my($mcom,$hp,$msub,$mbody);

	# メールタイトルを定義
	$msub = &base64("[$title : $no] $in{'sub'}");

	# 記事を復元
	$mcom  = $in{'comment'};
	$mcom =~ s/<br>/\n/g;
	$mcom =~ s/&lt;/</g;
	$mcom =~ s/&gt;/>/g;
	$mcom =~ s/&quot;/"/g;
	$mcom =~ s/&amp;/&/g;

	# URL情報
	if ($in{'url'}) { $hp = "http://$in{'url'}"; }
	else { $hp = ""; }

	# メール本文を定義
	$mbody = <<EOM;
投稿日時：$date
ホスト名：$host
ブラウザ：$ENV{'HTTP_USER_AGENT'}

投稿者名：$in{'name'}
Ｅメール：$in{'email'}
ＵＲＬ  ：$hp
タイトル：$in{'sub'}

$mcom
EOM

	# メールアドレスがない場合は管理者メールに置き換え
	if ($in{'email'} eq "") { $email = $mailto; }
	else { $email = $in{'email'}; }

	open(MAIL,"| $sendmail -t -i") || &error("送信失敗");
	print MAIL "To: $mailto\n";
	print MAIL "From: $email\n";
	print MAIL "Subject: $msub\n";
	print MAIL "MIME-Version: 1.0\n";
	print MAIL "Content-type: text/plain; charset=UTF-8\n";
	print MAIL "Content-Transfer-Encoding: 8bit\n";
	print MAIL "X-Mailer: $ver\n\n";
	foreach ( split(/\n/, $mbody) ) {
		print MAIL $_, "\n";
	}
	close(MAIL);
}

#--------------#
#  BASE64変換  #
#--------------#
#	とほほのWWW入門で公開されているルーチンを
#	参考にしました。( http://www.tohoho-web.com/ )
sub base64 {
	my($sub) = @_;

	$sub = "=?UTF-8?B?" . &b64enc($sub) . "?=";
	$sub;
}
sub b64enc {
	my($ch)="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	my($x, $y, $z, $i);
	$x = unpack("B*", $_[0]);
	for ($i=0; $y=substr($x,$i,6); $i+=6) {
		$z .= substr($ch, ord(pack("B*", "00" . $y)), 1);
		if (length($y) == 2) {
			$z .= "==";
		} elsif (length($y) == 4) {
			$z .= "=";
		}
	}
	$z;
}

#----------------------#
#  パスワード暗号処理  #
#----------------------#
sub encrypt {
	my($inpw) = $_[0];
	my(@SALT, $salt, $encrypt);

	@SALT = ('a'..'z', 'A'..'Z', '0'..'9', '.', '/');
	srand;
	$salt = $SALT[int(rand(@SALT))] . $SALT[int(rand(@SALT))];
	$encrypt = crypt($inpw, $salt) || crypt ($inpw, '$1$' . $salt);
	return $encrypt;
}

#----------------------#
#  パスワード照合処理  #
#----------------------#
sub decrypt {
	my($inpw, $logpw) = @_;
	my($salt, $check);

	$salt = $logpw =~ /^\$1\$(.*)\$/ && $1 || substr($logpw, 0, 2);
	$check = "no";
	if (crypt($inpw, $salt) eq $logpw || crypt($inpw, '$1$' . $salt) eq $logpw)
		{ $check = "yes"; }
	return $check;
}

#----------------#
#  HTMLヘッダー  #
#----------------#
sub header {
	$headflag=1;
	if ($outputhtml) {
		open(HOUT,">$htmlfile") || &error("Write Error : $htmlfile");
		 $|++; $cgiout=select(HOUT); $|++;
	}else{
		print "Content-type: text/html\n\n";
	}
	my %args=('{$title}'=>$title,'{$fields_name}'=>$fields{name},'{$fields_url}'=>$fields{url},'{$script}'=>$script,'{$dest}'=>$dest,
		'{$fields_email}'=>$fields{email},'{$cb_neme}'=>$cb_neme,'{$langstr}'=>$langstr,
		'{$jscol}'=>(!$nocolor)?"if(color)for(rc= 0; rc<color.length; rc++){if(jn[5]==color[rc].value){color[rc].checked=true;break;}}":'',
		'{$meta}'=>($_[0]==1)?"<META HTTP-EQUIV=\"Refresh\" CONTENT=\"2; URL=$dest\">":'');
	print $PTE->ParseBlock('header',\%args);
}

#-----------------#
#  自動URLリンク  #
#-----------------#
sub auto_link {
	$_[0] =~ s/([^=^\"]|^)(http\:[\w\.\~\-\/\?\&\+\=\:\@\%\;\#\%]+)/$1<a href=\"$2\" target='_blank'>$2<\/a>/g;
}

#----------------#
#  過去ログ生成  #
#----------------#
sub pastlog {
	my($past_flag)=0;

	# 過去NOを開く
	open(NO,"$nofile") || &error("Open Error : $nofile");
	$count = <NO>;
	close(NO);

	# 過去ログのファイル名を定義
	$pastfile  = "$pastdir$count\.dat";

	# 過去ログを開く
	open(IN,"$pastfile") || &error("Open Error : $pastfile");
	@past = <IN>;
	close(IN);

	# 規定の行数をオーバーすると次ファイルを自動生成
	if ($#past > $log_line) {
		$past_flag=1;

		# カウントファイル更新
		$count++;
		open(NO,">$nofile") || &error("Write Error : $nofile");
		print NO $count;
		close(NO);

		$pastfile = "$pastdir$count\.dat";
		@past=();
	}

	@temp=();
	foreach (@data) {
		($pno,$preno,$pdate,$pname,$pmail,$psub,$pcom,$purl,$pho)
								 = split(/<>/);
		if ($pmail) { $pname = "<a href=\"mailto:$pmail\">$pname</a>"; }
		if ($purl) { $purl = "&lt;<a href=\"http://$purl\" target='_top'>HOME</a>&gt;"; }
		if ($preno) { $pno = "Res: $preno"; }

		# 保存記事をフォーマット
		push(@temp,"<hr>[<b>$pno</b>] <font color=\"$sub_color\"><b>$psub</b></font> $jnstr_poster$jnstr_colon<b>$pname</b> $jnstr_postdate$jnstr_colon$pdate $purl<br><blockquote>$pcom</blockquote><!-- $pho -->\n");
	}

	# 過去ログを更新
	unshift(@past,@temp);
	open(OUT,">$pastfile") || &error("Write Error : $pastfile");
	print OUT @past;
	close(OUT);

	if ($past_flag) { chmod(0666,$pastfile); }
}

#------------#
#  過去ログ  #
#------------#
sub past {
	open(IN,"$nofile") || &error("Open Error : $nofile");
	$pastno = <IN>;
	close(IN);

	if (!$in{'pastlog'}) { $in{'pastlog'} = $pastno; }

	&header;
	print <<"EOM";
[<a href="$dest">$jnstr_backtobbs</a>]
<table width="100%"><tr><th bgcolor="#880000">
  <font color="#FFFFFF">$jnstr_pastlog[$in{'pastlog'}]</font>
</th></tr></table>
<p><table border=0><tr><td>
<form action="$script?$langstr" method="POST">
<input type=hidden name=mode value=past>
$jnstr_pastlog$jnstr_colon<select name=pastlog>
EOM

	$pastkey = $pastno;
	while ($pastkey > 0) {
		if ($in{'pastlog'} == $pastkey) {
			print "<option value=\"$pastkey\" selected>$pastkey Page\n";
		} else {
			print "<option value=\"$pastkey\">$pastkey Page\n";
		}
		$pastkey--;
	}
	print "</select>\n<input type=submit value='$jnstr_move'></td></form>\n";
	print "<td width=30></td><td>\n";
	print "<form action=\"$script?$langstr\" method=\"POST\">\n";
	print "<input type=hidden name=mode value=past>\n";
	print "<input type=hidden name=pastlog value=\"$in{'pastlog'}\">\n";
	print "$jnstr_find_keyword<input type=text name=word size=30 value=\"$in{'word'}\">\n";
	print "$jnstr_cond<select name=cond>\n";

	foreach ('AND', 'OR') {
		if ($in{'cond'} eq "$_") {
			print "<option value=\"$_\" selected>$_\n";
		} else {
			print "<option value=\"$_\">$_\n";
		}
	}
	print "</select>\n";
	print "$jnstr_display<select name=view>\n";
	if ($in{'view'} eq "") { $in{'view'} = $p_log; }
	foreach (5,10,15,20,25,30) {
		if ($in{'view'} eq "$_") {
			print "<option value=\"$_\" selected>$_件\n";
		} else {
			print "<option value=\"$_\">$_件\n";
		}
	}
	print "</select>\n<input type=submit value='$jnstr_search'></td></form>\n";
	print "</tr></table>\n";

	# 表示ログを定義
	$in{'pastlog'} =~ s/\D//g;
	$file = "$pastdir$in{'pastlog'}\.dat";

	# ワード検索処理
	if ($in{'word'} ne "") {
		$in{'word'} =~ s/　/ /g;
		@pairs = split(/\s+/, $in{'word'});

		@new=();
		open(IN,"$file") || &error("Open Error : $file");
		while (<IN>) {
			$flag=0;
			foreach $pair (@pairs) {
				if (index($_,$pair) >= 0) {
					$flag=1;
					if ($in{'cond'} eq 'OR') { last; }
				} else {
					if ($in{'cond'} eq 'AND') { $flag=0; last; }
				}
			}
			if ($flag) { push(@new,$_); }
		}
		close(IN);

		$count = @new;
		print "$jnstr_search_result<b>$count</b>$jnstr_post_n\n";
		if ($page eq '') { $page = 0; }
		$end_data = @new - 1;
		$page_end = $page + $in{'view'} - 1;
		if ($page_end >= $end_data) { $page_end = $end_data; }

		$next_line = $page_end + 1;
		$back_line = $page - $in{'view'};

		$enwd = &url_enc($in{'word'});
		if ($back_line >= 0) {
			print "[<a href=\"$script?mode=past&amp;page=$back_line&amp;word=$enwd&amp;view=$in{'view'}&amp;cond=$in{'cond'}&amp;pastlog=$in{'pastlog'}&amp;$langstr\">$jnstr_prevpage_pre$in{'view'}$jnstr_post_n</a>]\n";
		}
		if ($page_end ne "$end_data") {
			print "[<a href=\"$script?mode=past&amp;page=$next_line&amp;word=$enwd&amp;view=$in{'view'}&amp;cond=$in{'cond'}&amp;pastlog=$in{'pastlog'}&amp;$langstr\">$jnstr_nextpage_pre$in{'view'}$jnstr_post_n</a>]\n";
		}
		# 表示開始
		foreach ($page .. $page_end) { print $new[$_]; }
		print "<hr>\n</body></html>\n";
		exit;
	}

	# ページ区切り処理
	$start = $page + 1;
	$end   = $page + $p_log;

	$i=0;
	open(IN,"$file") || &error("Open Error : $file");
	while (<IN>) {
		$flag=0;
		if ($_ =~ /^\<hr\>\[\<b\>\d+\<\/b\>\]/) { $flag=1; $i++; }
		if ($i < $start) { next; }
		if ($i > $end) { last; }

		if ($flag) { print $_; }
		else {
			$_ =~ s/<hr>//ig;
			print "<blockquote>$_</blockquote>\n";
		}
	}
	close(IN);
	print "<hr>\n";

	$next = $page + $p_log;
	$back = $page - $p_log;
	print "<table>\n";
	if ($back >= 0) {
		print "<td><form action=\"$script\" method=\"POST\">\n";
		print "<input type=hidden name=mode value=past>\n";
		print "<input type=hidden name=pastlog value=\"$in{'pastlog'}\">\n";
		print "<input type=hidden name=page value=\"$back\">\n";
		print "<input type=submit value=\"$jnstr_prevpage_pre$p_log$jnstr_post_n\"></td></form>\n";
	}
	if ($next < $i) {
		print "<td><form action=\"$script\" method=\"POST\">\n";
		print "<input type=hidden name=mode value=past>\n";
		print "<input type=hidden name=pastlog value=\"$in{'pastlog'}\">\n";
		print "<input type=hidden name=page value=\"$next\">\n";
		print "<input type=submit value=\"$jnstr_nextpage_pre$p_log$jnstr_post_n\"></td></form>\n";
	}
	print "</table>\n</body>\n</html>\n";
	exit;
}

#------------------#
#  チェックモード  #
#------------------#
sub check {
	&header;
	print "<h2>Check Mode</h2>\n";
	print "<ul>\n";

	# ログパス
	if (-e $logfile) {
		print "<li>$jnstr_check_log$jnstr_check_path$jnstr_colon OK\n";
		# パーミッション
		if (-r $logfile && -w $logfile) {
			print "<li>$jnstr_check_log$jnstr_check_path$jnstr_check_permission$jnstr_check_path OK\n";
		} else { print "<li>$jnstr_check_log$jnstr_check_path$jnstr_check_permission$jnstr_check_path NG\n"; }
	} else { print "<li>$jnstr_check_log$jnstr_check_path$jnstr_check_path NG $jnstr_check_arrow $logfile\n"; }

	# カウンタログ
	print "<li>$jnstr_check_counter$jnstr_colon";
	if ($counter) {
		print "$jnstr_check_setted\n";
		if (-e $cntfile) { print "<li>$jnstr_check_counter$jnstr_check_log$jnstr_check_file$jnstr_check_path$jnstr_colon OK\n"; }
		else { print "<li>$jnstr_check_counter$jnstr_check_log$jnstr_check_file$jnstr_check_path$jnstr_colon NG $jnstr_check_arrow $cntfile\n"; }
	} else { print "$jnstr_check_notset\n"; }

	# ロックディレクトリ
	print "<li>$jnstr_check_lock$jnstr_check_mode$jnstr_colon";
	if ($lockkey == 0) { print "$jnstr_check_lock$jnstr_check_notset\n"; }
	else {
		if ($lockkey == 1) { print "symlink\n"; }
		else { print "mkdir\n"; }
		($lockdir) = $lockfile =~ /(.*)[\\\/].*$/;
		print "<li>$jnstr_check_lock$jnstr_check_dir$jnstr_colon$lockdir\n";

		if (-d $lockdir) {
			print "<li>$jnstr_check_lock$jnstr_check_dir$jnstr_check_path$jnstr_colon OK\n";
			if (-r $lockdir && -w $lockdir && -x $lockdir) {
				print "<li>$jnstr_check_lock$jnstr_check_dir$jnstr_check_permission$jnstr_colon OK\n";
			} else {
				print "<li>$jnstr_check_lock$jnstr_check_dir$jnstr_check_permission$jnstr_colon NG $jnstr_check_arrow $lockdir\n";
			}
		} else { print "<li>$jnstr_check_lock$jnstr_check_dir$jnstr_check_path$jnstr_colon NG $jnstr_check_arrow $lockdir\n"; }
	}

	# 画像ディレクトリ
	print "<li>$jnstr_check_img$jnstr_check_dir$jnstr_colon$imgdir\n";
	if (-d $imgdir) {
		print "<li>$jnstr_check_img$jnstr_check_dir$jnstr_check_path$jnstr_colon OK\n";
		if (-r $imgdir && -w $imgdir && -x $imgdir) {
			print "<li>$jnstr_check_img$jnstr_check_dir$jnstr_check_permission$jnstr_colon OK\n";
		} else {
			print "<li>$jnstr_check_img$jnstr_check_dir$jnstr_check_permission$jnstr_colon NG $jnstr_check_arrow $imgdir\n";
		}
	} else { print "<li>$jnstr_check_img$jnstr_check_dir$jnstr_check_path$jnstr_colon NG $jnstr_check_arrow $imgdir\n"; }

	# 過去ログ
	print "<li>$jnstr_pastlog$jnstr_colon";
	if ($pastkey == 0) { print "$jnstr_check_notset\n"; }
	else {
		print "$jnstr_check_setted\n";

		# NOファイル
		if (-e $nofile) {
			print "<li>NO$jnstr_check_file$jnstr_check_path$jnstr_colon OK\n";
			if (-r $nofile && -w $nofile) {
				print "<li>NO$jnstr_check_file$jnstr_check_permission$jnstr_colon OK\n";
			} else { print "<li>NO$jnstr_check_file$jnstr_check_permission$jnstr_colon NG $jnstr_check_arrow $nofile\n"; }
		} else { print "<li>NO$jnstr_check_file$jnstr_check_path$jnstr_colon NG $jnstr_check_arrow $nofile\n"; }

		# ディレクトリ
		if (-d $pastdir) {
			print "<li>$jnstr_pastlog$jnstr_check_dir$jnstr_check_path$jnstr_colon OK\n";
			if (-r $pastdir && -w $pastdir && -x $pastdir) {
				print "<li>$jnstr_pastlog$jnstr_check_dir$jnstr_check_permission$jnstr_colon OK\n";
			} else {
				print "<li>$jnstr_pastlog$jnstr_check_dir$jnstr_check_permission$jnstr_colon NG $jnstr_check_arrow $pastdir\n";
			}
		} else { print "<li>$jnstr_pastlog$jnstr_check_dir$jnstr_check_permission$jnstr_colon NG $jnstr_check_arrow $pastdir\n"; }
	}
	if(-e './resize.pl'){
		print "<LI>$jnstr_sampleresize$jnstr_colon$jnstr_have ".imgbbs::note();
	}else{
		print "<LI>$jnstr_sampleresize$jnstr_colon$jnstr_nothave";
	}
	print "</ul>\n</body></html>\n";
	exit;
}

#--------------#
#  ID生成処理  #
#--------------#

sub makeid {
	if ($genid==1) {$idcrypt="???";}
	else {
		$idnum = substr($addr, 8);
		$idcrypt = substr(crypt($idnum * $idnum, substr($date, 8, 2)), -8);
	}
}

#---------------------------#
#  2ch互換トリップ生成処理  #
#---------------------------#

sub tripping {
	$in{'name'} =~ s/◆/◇/g;
	$in{'name'} =~ s/★/☆/g;
	if ($in{'name'} =~ /(?:#|＃)(.+)$/) {
		my $key = $1;
		my $salt = substr(substr($key,1,2)."H.",0,2);
		$salt =~ s/[^\.-z]/\./go;
		$salt =~ tr/:;<=>?@[\\]^_`/ABCDEFGabcdef/;
		my $str = substr(crypt($key, $salt),-10);
		$in{'name'} =~ s/(#|＃).+$/<span class="nor">◆$str<\/span>/;
	}
	&Capping;
}

sub Capping {
	if ($in{'email'} =~ /(?:#|＃)(.+)$/) {
		my $flg;
		my $capkey=$1;
		my $basename=$in{'name'};
		$basename=~s/<span class="nor">◆(.+)$//g;
		open(FH,"$capfile") || &error("Open Error : $capfile");
		@cap = <FH>;
		close(FH);
		foreach (@cap) {
			@capdata = split(/<>/,$_);
			if($basename =~ /^$capdata[0]$/ && decrypt($capkey,$capdata[1]) eq "yes") {
				$in{'name'} = "<font color='".$capdata[2]."'>".$in{'name'}." <span class=\"nor\">★".$capdata[3]."</span></font>";
				$in{'email'} =~ s/(#|＃).+$//;
				$flg=1;last;
			}
		}
		&error($jnstr_invalidcap) if(!$flg);
	}
}

#------------#
#  sage機能  #
#------------#

sub sage {
	$s_flag = 0;
	$a_flag = 0;
	if ($sagemode && $in{'email'} =~ /^sage/) { $s_flag = 1; }
	if ($sagemode && $in{'email'} =~ /^age/) { $a_flag = 1; }
}

#------------------#
#  JPEGサイズ認識  #
#------------------#
sub JpegSize {
	my($jpeg) = @_;
	my($t, $m, $c, $l, $W, $H);

	open(JPEG,"$jpeg") || return (0,0);
	binmode JPEG;
	read(JPEG, $t, 2);
	while (1) {
		read(JPEG, $t, 4);
		($m, $c, $l) = unpack("a a n", $t);

		if ($m ne "\xFF") { $W = $H = 0; last; }
		elsif ((ord($c) >= 0xC0) && (ord($c) <= 0xC3)) {
			read(JPEG, $t, 5);
			($H, $W) = unpack("xnn", $t);
			last;
		}
		else {
			read(JPEG, $t, ($l - 2));
		}
	}
	close(JPEG);
	return ($W, $H);
}

#-----------------#
#  GIFサイズ認識  #
#-----------------#
sub GifSize {
	my($gif) = @_;
	my($data);

	open(GIF,"$gif") || return (0,0);
	binmode(GIF);
	sysread(GIF,$data,10);
	close(GIF);

	if ($data =~ /^GIF/) { $data = substr($data,-4); }

	$W = unpack("v",substr($data,0,2));
	$H = unpack("v",substr($data,2,2));
	return ($W, $H);
}

#-----------------#
#  PNGサイズ認識  #
#-----------------#
sub PngSize {
	my($png) = @_;
	my($data);

	open(PNG, "$png") || return (0,0);
	binmode(PNG);
	read(PNG, $data, 24);
	close(PNG);

	$W = unpack("N", substr($data, 16, 20));
	$H = unpack("N", substr($data, 20, 24));
	return ($W, $H);
}

#-----------------#
#  URLエンコード  #
#-----------------#
sub url_enc {
	local($_) = @_;

	s/(\W)/'%' . unpack('H2', $1)/eg;
	s/\s/+/g;
	$_;
}
#========================= New Log Processing Codes Below =========================#
sub loadlog {
	my $lgfile = $_[0]; # log file
	my $lg;
	%logs = (); # create an empty hash for logs
	%trees = (); # create an empty hash for tree
	@torder = (); # empty tree order scaler
	@porder = (); # empty post order scaler

	open(IN,"$lgfile") || &error("Open Error : $lgfile");
	$top = <IN>; # first line is counter

	# build log/tree hash
	while (<IN>) {
	   ($no,$rno,$lg) = split(/<>/);
	   $logs{$no} = [ split(/<>/) ];
   	   push(@porder,$no);
	   if (!$rno) {
	   	   push(@torder,$no);
	   		$trees{$no}= [ $no ];
	   } elsif (!defined $trees{$rno}) {
	   	   push(@torder,$rno);
	   		$trees{$rno}= [ $no ];
	   } else {
	   	   push(@{$trees{$rno}},$no);
	   }
	}
}

sub savelog {
	my $lgfile = $_[0]; # log file
	my $cnt=0;
	@pdata=();
	open(OUT,">$lgfile") || &error("Write Error : $lgfile");

	print OUT $top;

	# rebuild file from tree log
	for (my $i=0;$i<=$#torder;$i++) {
		for (my $j=0;$j<=$#{$trees{$torder[$i]}};$j++) {
			$cnt++;
			if ($cnt<$max) {
				print OUT join("<>", @{$logs{$trees{$torder[$i]}[$j]}});
			} else {
				push(@pdata,join("<>", @{$logs{$trees{$torder[$i]}[$j]}})) if($pastkey);
				my $ft2=$logs{$trees{$torder[$i]}[$j]}[15];
				my $tail2=$logs{$trees{$torder[$i]}[$j]}[11];
				if (-e "$imgdir$ft2$tail2") { unlink("$imgdir$ft2$tail2"); }
				if (-e "${imgdir}$ft2"."s.jpg") { unlink("${imgdir}$ft2"."s.jpg"); }
			}
		}
	}
	close(OUT);
}

sub tSearch {
	my $p=$_[0];
	for (my $i=0;$i<=$#torder;$i++) {
		return $i if ($torder[$i] == $p);
	}
	return 0;
}

sub getIndex {
	my $ce = shift(@_);
	my @arr = @_;
	my $idx;
	for (my $i=0;$i<=$#arr;$i++) {
		if ($ce eq $arr[$i]) { $idx=$i; last; }
	}
	$idx;
}

sub bumppost {
	my $idx=$_[0];
	my $age=splice(@torder, $idx, 1);
	unshift(@torder,$age);
}

sub removepost {
	my @p = @_;
	my $idx;
	my @posts=();
	foreach (@p) {
		if ($logs{$_}[1]) { # not root post
			$idx=&getIndex($_,@{$trees{$logs{$_}[1]}});
			push(@posts,splice(@{$trees{$logs{$_}[1]}}, $idx, 1));
		} else { # root post
			$idx=&getIndex($_,@torder);
			push(@posts,splice(@torder, $idx, 1));
		}
	}
	&removeattachment(@posts);
}
sub removeattachment {
	my @p = @_;
	foreach (@p) {
		my $ft2=$logs{$_}[15];
		my $tail2=$logs{$_}[11];
		$logs{$_}[11]="";
		if (-e "$imgdir$ft2$tail2") { unlink("$imgdir$ft2$tail2"); }
		if (-e "${imgdir}$ft2"."s.jpg") { unlink("${imgdir}$ft2"."s.jpg"); }
	}
}
#========================= New Log Processing Codes Adove =========================#

#----------------#
#  投稿フォーム  #
#----------------#
sub form {
	my($type,$no,$reno,$date,$name,$mail,$sub,$com,$url,$host,$pw,$color) = @_;
	my($cnam,$ceml,$curl,$cpwd,$cico,$ccol);

	my %args=('{$jnstr_name}'=>$jnstr_name,'{$jnstr_email}'=>$jnstr_email,'{$jnstr_title}'=>$jnstr_title,'{$jn_notes}'=>'','{$clip}'=>'',
		'{$jnstr_commets}'=>$jnstr_commets,'{$script}'=>$script,'{$langstr}'=>$langstr,'{$'.(!$type?'new':$type).'mode}'=>1);

	%args = (%args, ('{$form_title}'=>${'jnstr_'.(!$type?'new':$type).'_mode'})) if(${'jnstr_'.(!$type?'new':$type).'_mode'});
	## フォーム種別を判別
	# 修正
	if ($type eq "edit") {
		%args = (%args, ('{$hiddens}'=>"<input type=hidden name=mode value=\"usr_edt\"><input type=hidden name=action value=\"edit\"><input type=hidden name=pwd value=\"$in{'pwd'}\"><input type=hidden name=no value=\"$in{'no'}\">"));
		$cnam = $name;
		$ceml = $mail;
		$curl = $url;
		$ccol = $color;
	# 新規 / 返信
	} else {
		%args = (%args, ('{$hiddens}'=>'<input type=hidden name=mode value="regist">'.(($type eq "res")?"<input type=hidden name=reno value=\"$in{'no'}\">":'')));
	}

	%args = (%args, ('{$jn_name_field}'=>"<input type=text name=$traps[0] size=1 class='h'><input type=text name=$fields{name} size=28 value='$cnam'>",
		'{$jn_email_field}'=>"<input type=text name=$traps[1] size=1 class='h'><input type=text name=$fields{email} size=28 value='$ceml'>",
		'{$jn_sub_field}'=>"<input type=text name=$traps[2] size=1 class='h'><input type=text name=$fields{sub} size=28 value='$sub'>",
		'{$jn_submit_button}'=>"<input type=submit value='$jnstr_submit'>",
		'{$jn_reset_button}'=>"<input type=reset value='$jnstr_reset'>",
		'{$jn_comment_field}'=>"<textarea cols=1 rows=1 name=$traps[3] class='h'></textarea><textarea cols=56 rows=7 name=$fields{comment}>$com</textarea>",
		'{$jn_url_field}'=>"<input type=text name=$traps[4] size=1 class='h'><input type=text size=50 name=$fields{url} value='http://$curl'>",));


	# 添付フォーム
	if ($clip && ($type eq "" || ($type eq "res" && $res_clip))) {
		%args = (%args, ('{$clip}'=>'enctype="multipart/form-data"','{$jnstr_attachment}'=>$jnstr_attachment,'{$jnstr_attachment_field}'=>'<input type=file name=upfile size=40>'));
	}
	# パスワード欄
	if ($type ne "edit") {
		%args = (%args, ('{$jnstr_delkey}'=>$jnstr_delkey,'{$jnstr_delkey_field}'=>"<input type=password name=pwd size=8 maxlength=8 value=\"$cpwd\"> ",'{$jnstr_delkey_post}'=>$jnstr_delkey_post));
	}
	
	# 色指定

	my @colors = ();

	if (!$nocolor) {
		@col = split(/\s+/, $colors);
		if ($ccol eq "") { $ccol = $col[0]; }
		foreach (@col) {
				push @colors,{'{$color_button}'=>"<input type=radio name=color value=\"$_\"".(($ccol eq $_)?' checked':'').">",'{$color}'=>$_};
		}
		%args = (%args, ('{$jnstr_txtcol}'=>$jnstr_txtcol,'{$jn_color}'=>\@colors));
	} else {
		%args = (%args, ('{$jn_color}'=>"<input type=hidden name=color value=\"$tx\">"));
	}

	if ($ImageCheck) {
		$args{'{$jn_notes}'} .= $jnstr_chkimg;
	}
	if ($admin_note && !$type) {
		$args{'{$jn_notes}'} .= $admin_note;
	}
	return $PTE->ParseBlock('form',\%args);
}
__END__
