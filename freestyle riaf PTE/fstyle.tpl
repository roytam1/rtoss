<!--&MAIN-->
<html>
<head>
<title>{$tit}</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<META HTTP-EQUIV="pragma" CONTENT="no-cache">
<link rel="alternate" type="application/rss+xml" title="RSS" href="backend.php">
</head>
<head><script language="JavaScript"><!-- 
function l(e){var N=loadCookie("NAME"),M=loadCookie("MAIL"),P=loadCookie("PASS"),i;with(document)for(i=0;i<forms.length;i++)if(forms[i].nick&&forms[i].mail)with(forms[i]){nick.value=N;mail.value=M; forms[i].delk?delk.value=P:P=P;} li();}onload=l;
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

function li(e){var ui=loadCookie("interface");if(ui=="2ch") location.href="{$NIch_index}?";else w("fstyle");}
function w(i){setCookie("interface",i);};function setCookie(name,value){var exp=new Date();exp.setTime(exp.getTime()+(1000*60*60*24*7));document.cookie = name+"="+escape(value)+"; expires="+exp.toGMTString(); }
//--></script>
<style type="text/css">
<!--
BODY,TD,SELECT,input,DIV,form,TEXTAREA,center,option,pre,blockquote {
font-size:10pt; font-family:"ＭＳ Ｐゴシック", "Osaka"; color:002a45}
A:link    {color:#0000C0;text-decoration:none;}
A:visited {color:#8000D0;text-decoration:none;}
A:active  {color:blue;text-decoration:none;}
A:hover  {color:#000080;background-color:#fafad2;}
.input {border:solid 1;background-color:white;}
.submit {border:solid 1 #122173;font-family:Tahoma,Osaka,Verdana;font-size:8pt;color:white;background-color:#7281C3; height:18px}
-->
</style>
</head>
<body bgcolor="#ffffff">
<center>
<table border="1" bordercolor="3582c1" cellpadding="0" cellspacing="0" width="90%">
  <tbody>
    <tr>
      <td bgcolor="3582c1"><a name="top"></a>
      <table border="0" cellpadding="3" cellspacing="0" width="100%">
        <tbody>
          <tr>
            <td bgcolor="CEDEEF" height="25">&nbsp;</td>
          </tr>
          <tr>
            <td bgcolor="EFF5FF">
            <font color="#004080" size="+2">{$tit}</font>
            <blockquote><font color="green">{$rmes}<br>{$tmes}
            </font></blockquote>
<!--&NEWFORM/-->
            </td>
          </tr>
          <tr>
            <td bgcolor="CEDEEF" height="25" align="right"><a href="{$NIch_index}?" onclick="w('2ch');">2chインタフェース</a> | <a href="search.php">検索</a> | <a href="help.html" target="_blank">独自タグがつかえます</a> | <a href="./i/">携帯</a> | <a href="admin.php">管理</a>&nbsp;</td>
          </tr>
        </tbody>
      </table>
      </td>
    </tr>
  </tbody>
</table>
<br>
<form></form>
<table cellpadding="10" cellspacing="0" bgcolor="#EFF5FD" width="90%" border="1" bordercolor="3582c1">
  <tbody>
    <tr>
      <td><font size="2"><a name="headline"></a>
      <table border="0" cellpadding="1" cellspacing="0" align="center" valign="top" width="99%">
        <tbody>
          <tr>
<!--&FOREACH($head,'HEAD')-->
<!--&FOREACH($over,'HEADOVER')-->
          </tr>
        </tbody>
      </table>
      <small><!--&PAGES/-->&emsp;<a href="subback.php">スレッド一覧はこちら</a></small> </font></td>
    </tr>
  </tbody>
</table>
<br>
<!--&FOREACH($oya,'THREAD')-->
<!--&IF($PAGEBAR,'<!--&PAGEBAR/-->','')-->
<hr>
<div align="right"><a href="http://php.s3.to" target="_blank">freeStyle bbs byレッツPHP!</a> + <a href="http://riaf.org/f_style/">riaf web</a> + PTE</div>
</center>
</body>
</html>
<!--/&MAIN-->

<!--&HEAD-->
            <td align="left" width="33%"><a href="./read.php/{$key}/l{$read_def}"><b>{$no}</b></a> :<a href="#{$no}">{$title}</a></td>
<!--&IF($tr,'</tr><tr>','')-->
<!--/&HEAD-->

<!--&HEADOVER-->
            <td align="left" width="33%"><a href="./read.php/{$key}/l{$read_def}"><b>{$no}</b> :{$title}</a></td>
<!--&IF($tr,'</tr><tr>','')-->
<!--/&HEADOVER-->

<!--&THREAD-->
<table border="1" bordercolor="3582c1" cellpadding="0" cellspacing="0" width="90%">
  <tbody>
    <tr>
      <td bgcolor="3582c1">
      <table border="0" cellpadding="1" cellspacing="0" width="100%">
        <tbody>
          <tr bgcolor="94B4DA">
            <td height="23"><a name="{$no}"></a><b>#{$no} </b>{$fdate}&nbsp;[ <!--&IF($fmaillink,'<a href="mailto:{$femail}">{$fname}</a>','{$fname}')--> ] {$fhost}</td>
            <td align="right">Res {$resnum} <a href="#top">■</a><a href="#{$next}">▼</a><a href="#{$prev}">▲</a></td>
          </tr>
          <tr bgcolor="CEDFEF">
            <td height="32" colspan="2">&nbsp;1:<font color="335588" size="3"><b> {$fsub}</b></font></td>
          </tr>
          <tr bgcolor="F3F9FF">
            <td colspan="2"><br>
            <blockquote>{$fcom}</blockquote>
            <br>
<!--&IF($skip,'<center><font color="indigo">（レス {$skip} 件省略されました。全て読むには<a href="read.php/{$key}/" target="_blank"><u>ここ</u></a>をクリック！)</font></center>','')-->
            <table border="0" cellspacing="2" cellpadding="0" width="100%">
              <tbody>
<!--&FOREACH($res,'RES')-->
              </tbody>
            </table>
            <br>
            </td>
          </tr>
<!--&IF($stopped,'','<!--&RESFORM/-->')-->
          <tr height="30" bgcolor="#94B4DA">
            <td align="right" colspan="2"><font size="-1"><b><a href="./read.php/{$key}/">全レス</a> <a href="./read.php/{$key}/l{$read_def}">最新レス{$read_def}</a> <a href="./read.php/{$key}/1-100">レス1-100</a> <a href="#top">Top</a> <a href="./usr_del.php/{$key}/">ResDelete</a> <a href="?">Reload</a></b></font></td>
          </tr>
        </tbody>
      </table>
      </td>
    </tr>
  </tbody>
</table>
<br>
<!--/&THREAD-->

<!--&RESSHOW-->
                <tr>
                  <td height="1" colspan="3">
                  <hr size="1">
                  </td>
                </tr>
                <tr valign="top">
                  <td width="140" nowrap>{$no}: <font color="navy"><b><!--&IF($maillink,'<a href="mailto:{$email}">{$name}</a>','{$name}')--></font></b><br>
                  <br>
                  <small>{$now}<br>
                  {$host}</small></td>
                  <td>{$com} <!--&IF($over,'<font color="indigo">（省略されました。全て読むには<a href="read.php/{$key}/{$no}-{$no}n" target="_blank"><u>ここ</u></a>をクリック！)</font>','')--></td>
                  <td></td>
                </tr>
<!--/&RESSHOW-->

<!--&RES-->
<!--&IF($show,'<!--&RESSHOW/-->','')-->
<!--/&RES-->

<!--&RESFORM-->
          <form method="post" action="./bbs.php"><input type="hidden" name="key" value="{$key}">
          <tr>
            <td bgcolor="C0D5EB" colspan="2">&nbsp; <b>Name</b> <input type="text" value="" name="nick" size="20" maxlength="40" class="input"> <b>E-Mail</b> <input type="text" value="" name="mail" size="15" class="input"> <br>
            &nbsp;&nbsp;<textarea name="content" cols="75" rows="3" class="input" wrap="soft"></textarea> <br>
            &nbsp;&nbsp;<b>Password</b> <input type="password" value="" name="delk" size="15" class="input"><span style="display:none;"> URL（省略必要）<input type="text" value="" name="url" size="19" class="input"></span> <input type="submit" value=" Write " class="submit"></td>
          </tr>
          </form>
<!--/&RESFORM-->

<!--&PAGEBAR-->
<!--&PAGES/-->
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
            <span id="newform">
            <form method="post" action="./bbs.php"><font size="-1">名前&nbsp; <input type="text" name="nick" size="20" class="input" value="">　 メール <input type="text" name="mail" size="30" class="input" value=""><br>
            題名&nbsp; <input type="text" name="subject" class="input" maxlength="80" size="50"> <input type="submit" value="新規投稿"> <b><a href="?">リロード</a> </b><br>
            本文&nbsp; <textarea name="content" rows="5" class="input" cols="70"></textarea></font>
			<span style="display:none;"> URL（省略必要）<input type="text" value="" name="url" size="19" class="input"></span>
			</form>
            </span>
<!--/&NEWFORM-->
