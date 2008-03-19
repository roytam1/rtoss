<!--&MAIN-->
<html><head><title>freeStyle</title>
<body>
{$tit} <br>
<hr>

<!--&IF($post,'<!--&POST/-->','')-->
<!--&IF($main,'<!--&MAINFRAME/-->','')-->

<br><div align=right><small><a href=http://php.s3.to target=_blank>freeStyle bbs</a> / <a href="http://riaf.org/f_style/">riaf</a> / PTE</small></div>
</center></body></html>
<!--/&MAIN-->

<!--&POST-->
<form method="post" action="{$dir_path}../bbs.php">
名前<br>
<input type="text" name="nick" size="20"><br>
メール<br>
<input type="text" name="mail" size="30"><br>
題名<br>
<input type="text" name="subject" maxlength="80" size="50"><br>
<textarea name="content" rows="5" class=input cols="70"></textarea><br>
験証<br>
以下の入力ﾌｨｰﾙﾄﾞをｸﾘｱしてください｡<br>
<input type=text value="x" name=url size=8><br>
<input type="submit" value="新規投稿">
</form>
<!--/&POST-->

<!--&MAINFRAME-->
<a href="i.php?mode=post">新規投稿</a><br>
<!--&FOREACH($head,'HEAD')-->

<hr>
<!--&IF($prev,'<a href="i.php?page={$prev}">←前へ</a>　','')-->
<!--&IF($next,'<a href="i.php?page={$next}">次へ→</a>','')-->
<!--/&MAINFRAME-->

<!--&HEAD-->
{$no} : <a href="{$dir_path}read.php?key={$key}&ls={$read_def}&ua=i">{$title}</a><br>
<!--/&HEAD-->
