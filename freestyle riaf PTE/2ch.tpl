<!--&MAIN--><html>
<head>
<title>{$tit}</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<META HTTP-EQUIV="pragma" CONTENT="no-cache">
<link rel="alternate" type="application/rss+xml" title="RSS" href="backend.php">
<script language="JavaScript"><!-- 
function l(e){var N=loadCookie("NAME"),M=loadCookie("MAIL"),P=loadCookie("PASS"),i;with(document)for(i=0;i<forms.length;i++)if(forms[i].nick&&forms[i].mail)with(forms[i]){nick.value=N;mail.value=M;forms[i].delk?delk.value=P:P=P;} li();}onload=l;
/* Function Equivalent to URLDecoder.decode(String, "UTF-8")
   Copyright (C) 2002 Cresc Corp. http://www.cresc.co.jp/
   Version: 1.0
*/
function decodeURL(str){
	var s0, i, j, s, ss, u, n, f;
	s0 = "";
	for (i = 0; i < str.length; i++){
		s = str.charAt(i);
		if (s == "+"){s0 += " ";}
		else {
			if (s != "%"){s0 += s;}
			else{
				u = 0;
				f = 1;
				while (true) {
					ss = "";
					for (j = 0; j < 2; j++ ) {
						sss = str.charAt(++i);
						if (((sss >= "0") && (sss <= "9")) || ((sss >= "a") && (sss <= "f"))  || ((sss >= "A") && (sss <= "F"))) {
							ss += sss;
						} else {--i; break;}
					}
					n = parseInt(ss, 16);
					if (n <= 0x7f){u = n; f = 1;}
					if ((n >= 0xc0) && (n <= 0xdf)){u = n & 0x1f; f = 2;}
					if ((n >= 0xe0) && (n <= 0xef)){u = n & 0x0f; f = 3;}
					if ((n >= 0xf0) && (n <= 0xf7)){u = n & 0x07; f = 4;}
					if ((n >= 0x80) && (n <= 0xbf)){u = (u << 6) + (n & 0x3f); --f;}
					if (f <= 1){break;}
					if (str.charAt(i + 1) == "%"){ i++ ;}
					else {break;}
				}
				s0 += String.fromCharCode(u);
			}
		}
	}
	return s0;
}

/* Function to get cookie parameter value string with specified name
   Copyright (C) 2002 Cresc Corp. http://www.cresc.co.jp/
   Version: 1.0
*/
function loadCookie(name) {
	var allcookies = document.cookie;
	if (allcookies == "") return "";
	var start = allcookies.indexOf(name + "=");
	if (start == -1) return "";
	start += name.length + 1;
	var end = allcookies.indexOf(';',start);
	if (end == -1) end = allcookies.length;
	return decodeURL(allcookies.substring(start,end));
}
function li(e){var P=loadCookie("interface");if(P=="fstyle") location.href="{$pc_index}?";else w("2ch");}
function w(i){setCookie("interface",i);};function setCookie(name,value){var exp=new Date();exp.setTime(exp.getTime()+(1000*60*60*24*7));document.cookie = name+"="+escape(value)+"; expires="+exp.toGMTString(); }
//--></script>
<style type="text/css">
<!--
dt,dd {
font-size:12pt; font-family:"ＭＳ Ｐゴシック", "Osaka"}
-->
</style>
</head>
<body bgcolor=#efefef text=#000000 link=#0000ff alink=#ff0000 vlink=#aa0088 background="ba.gif">
<center><a name="top" />{$kanban}<br>
<table border=1 cellspacing=7 cellpadding=3 width=95% bgcolor=#ccffcc><tr><td><table border=0 width=100%><tr><td>
<b><big>{$tit}</big></b></td><td align=right><a href=#menu>■</a><a name="0"></a><a href=#1>▼</a></td></tr><tr><td colspan=2>
<center><br><font color="red">{$rmes}<br>{$tmes}</font>
<br><br></b></center><small></td></tr></table></td></tr><tr><td align="center"><small><a href="{$pc_index}?" onclick="w('fstyle');">FreeStyleインタフェース</a> | <a href="#new">新規作成</a> | <a href="search.php">検索</a> | <a href="help.html" target="_blank">独自タグがつかえます</a> | <a href="./i/">携帯</a> | <a href="admin.php">管理</a></small></td></tr></table><br>
<a name="menu" /><table border=1 cellspacing=7 cellpadding=3 width=95% bgcolor=#ccffcc><tr><td><small>
<!--&FOREACH($head,'HEAD')-->
<!--&FOREACH($over,'HEADOVER')-->
      <br><div style="float:left"><!--&PAGES/--></div><div align="right"><a href="subback.php">スレッド一覧はこちら</a></div></small></td>
    </tr>
</table>
<br>
<!--&FOREACH($oya,'THREAD')-->
<!--&IF($PAGEBAR,'<!--&PAGEBAR/-->','')-->

<!--&NEWFORM/-->

<center>- <a href="http://php.s3.to" target="_blank">freeStyle bbs byレッツPHP!</a> + <a href="http://riaf.org/f_style/">riaf web</a> + PTE -</center>
</center>
</body>
</html>
<!--/&MAIN-->

<!--&HEAD-->
            <a href="./read.php/{$key}/l{$read_def}"><b>{$no}</b></a> :<a href="#{$no}">{$title}</a>　
<!--/&HEAD-->

<!--&HEADOVER-->
            <a href="./read.php/{$key}/l{$read_def}"><b>{$no}</b> :{$title}</a>　
<!--/&HEADOVER-->

<!--&THREAD-->
<table border=1 cellspacing=7 cellpadding=3 width=95% bgcolor=#efefef>
  <tr><td><dl><a name="{$no}"></a><div align="right" style="float:right"><a href="#top">■</a><a href="#{$next}">▼</a><a href="#{$prev}">▲</a></div>
  <b>【{$no}:{$resnum}】<font size=+2 color=#ff0000>{$fsub}</font></b>
            <dt>1 名前：<font color=green><b><!--&IF($fmaillink,'<a href="mailto:{$femail}">{$fname}</a>','{$fname}')--></b></font>：{$fdate} {$fhost}</dt>
            <dd>{$fcom}<br><br><!--&IF($skip,'<font color=green>（レス {$skip} 件省略されました…全て読むには<a href="read.php/{$key}/" target="_blank"><u>ここ</u></a>を押してください)</font>','')--></dd><br>
<!--&FOREACH($res,'RES')-->
            <br>
<!--&IF($stopped,'','<!--&RESFORM/-->')-->
<b><a href="./read.php/{$key}/">全部読む</a> <a href="./read.php/{$key}/l{$read_def}">最新{$read_def}</a> <a href="./read.php/{$key}/1-100">1-100</a> <a href="#top">板のトップ</a> <a href="./usr_del.php/{$key}/">レス削除</a> <a href="?">リロード</a></b></font>
      </td>
    </tr>
  </tbody>
</table>
<br>
<!--/&THREAD-->

<!--&RESSHOW-->
  <dt>{$no} ：<font color="forestgreen"><b><!--&IF($maillink,'<a href="mailto:{$email}">{$name}</a>','{$name}')--></b></font> ：{$now} {$host}</dt>
  <dd>{$com}<br>
<!--&IF($over,'<font color=green>（省略されました…全てを読むには<a href="read.php/{$key}/{$no}-{$no}n" target="_blank"><u>ここ</u></a>を押してください）</font>','')--></dd><br>
<!--/&RESSHOW-->

<!--&RES-->
<!--&IF($show,'<!--&RESSHOW/-->','')-->
<!--/&RES-->

<!--&RESFORM-->
          <form method="post" action="./bbs.php"><input type="hidden" name="key" value="{$key}">
<input type=submit value="書き込む" name=submit>
 名前：<input type=text name=nick size=19 maxlength=40>
 E-mail：<input type=text name=mail size=19>
 削除キー：<input type=password name=delk size=8 maxlength=8>
<span style="display:none;"> ＵＲＬ：（省略必要）<input type="text" value="" name="url" size="19" class="input"></span><br>
<ul><textarea rows=5 cols=64 wrap=off name="content"></textarea><br>
</form>
<!--/&RESFORM-->

<!--&PAGEBAR-->
<table border=1 cellspacing=7 cellpadding=3 bgcolor=#ccffcc align=center>
<tr><td>
<!--&PAGES/-->
</td></tr></table><br/>
<!--/&PAGEBAR-->

<!--&PAGES-->
<!--&IF($PREVPAGE,'<a href="page.php?page={$PREVPAGE}">＜＜前のページ</a> ','')-->
<!--&FOREACH($PAGES,'PAGE')-->
<!--&IF($NEXTPAGE,'<a href="page.php?page={$NEXTPAGE}">次のページ＞＞</a> ','')-->
<!--/&PAGES-->

<!--&PAGE-->
<!--&IF($THISPAGE,'<b>[{$PAGE}]</b> ','<a href="page.php?page={$PAGE}">[{$PAGE}]</a> ')-->
<!--/&PAGE-->

<!--&NEWFORM-->
<form method="post" action="./bbs.php"><a name="new"></a>
<div id="newform">
<table border=1 cellspacing=7 cellpadding=3 width=95% bgcolor=#ccffcc><tr><td><table border=0 width=100%><tr><td><div align="right" style="float:right"><a href="#top">■</a></div>
<font size=-1>名前&nbsp; <input type="text" name="nick" size="20" class=input value="">　
メール <input type="text" name="mail" size="30" class=input value=""><br>
題名&nbsp; <input type="text" name="subject" class=input maxlength="80" size="50">
<input type="submit" value="新規スレッド作成">
<b><a href="?">リロード</a> </b><br>
本文&nbsp; <textarea name="content" rows="5" class=input cols="70"></textarea>
<span style="display:none;"> ＵＲＬ：（省略必要）<input type="text" value="" name="url" size="19" class="input"></span>
</font>
</td></tr></table></td></tr></table></div></form>
<!--/&NEWFORM-->
