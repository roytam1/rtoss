<!--&READ-->
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>{$fsub}</title>
<link rel="alternate" type="application/rss+xml" title="RSS" href="{$dir_path}backend.php/{$key}/">
</head>
<body bgcolor="#efefef" text="#000000" link="#0000ff" alink="#ff0000" vlink="#aa0088">
<p><a name="saisyo"></a><a href="{$dir_path}">■掲示板に戻る■</a> <a href="{$PHP_SELF}/{$key}/">全部</a> <!--&FOREACH($page,'PAGES')-->
 <a href="{$PHP_SELF}/{$key}/l50">最新50</a> <a href="{$dir_path}backend.php?thread_id={$key}">RSS</a> <a href="{$dir_path}usr_del.php/{$key}/">レス削除</a></p>
<p><!--&IF($notice_err,'<b style="background-color:yellow;color:black;">レスが {$notice} を超えてます。{$numlimit} を超えると書き込み出来なくなるよ</b><br>','')--><!--&IF($over_err,'<b style="background-color:red;color:white;">レスが {$numlimit} を超えてます。残念ながら全部は表示しません</b><br>','')--></p>
<dl><font color="#ff0000" size="+1">{$fsub}</font><br>
  <br>
<!--&IF($first,'<!--&FIRST/-->','')-->
<!--&FOREACH($res,'RES')-->
</dl>
<a name="saigo"></a><font color="red" face="Arial"><b>{$size}KB</b></font>
<hr>
<center><a href="{$PHP_SELF}/{$key}/{$new}-">新着レスの表示</a></center>
<hr>
<a href="{$home}">掲示板に戻る</a> <a href="{$PHP_SELF}/{$key}/">全部</a> <!--&IF($prev,'<a href="{$PHP_SELF}/{$key}/{$prev_st}-{$prev}">前{$read_def}</a>','')--> <!--&IF($next,'<a href="{$PHP_SELF}/{$key}/{$num}-{$next}">次{$read_def}</a>','')--> <a href="#saisyo">↑先頭</a> <a href="{$dir_path}usr_del.php/{$key}/">レス削除</a>
<!--&IF($stopped,'','<!--&RESFORM/-->')-->
<p><font size="-2"><a href="http://php.s3.to">read.php ver2 (2004/1/26)</a> + <a href="http://riaf.org/f_style/">riaf web</a> + PTE</font></p>
</body>
</html>
<!--/&READ-->

<!--&FIRST-->
  <dt>1 ：<font color="forestgreen"><b><!--&IF($femail,'<a href="mailto:{$femail}">{$fname}</a>','{$fname}')--></b></font> ：{$fdate}
  <dd>{$fcom}<br>
  <br>
<!--/&FIRST-->

<!--&RESSHOW-->
  <dt>{$num} ：<font color="forestgreen"><b><!--&IF($email,'<a href="mailto:{$email}">{$name}</a>','{$name}')--></b></font> ：{$date}
  <dd>{$com}<br>
  <br>
<!--/&RESSHOW-->

<!--&RES-->
<!--&IF($show,'<!--&RESSHOW/-->','')-->
<!--/&RES-->

<!--&PAGES-->
<a href="{$PHP_SELF}/{$key}/{$st}-{$to}">{$st}-</a> 
<!--/&PAGES-->

<!--&RESFORM-->
<form method="POST" action="{$dir_path}bbs.php"><input type="hidden" name="key" value="{$key}"><input type="submit" value="書き込む"> 名前：<input type="text" value="{$cname}" name="nick" size="19" maxlength="40" class="input"> E-mail<font size="1">（省略可）</font>： <input type="text" value="{$cmail}" name="mail" size="19" class="input"> PASS：<input type="password" value="{$cpass}" name="delk" size="8" maxlength="8" style="font-size:12px"><span style="display:none;"> ＵＲＬ：（省略必要）<input type="text" value="" name="url" size="19" class="input"></span><br>
<textarea rows="5" cols="70" wrap="off" name="content"></textarea><br>
</form>
<!--/&RESFORM-->
