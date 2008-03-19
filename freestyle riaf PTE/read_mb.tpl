<!--&READ-->
<title>{$fsub}</title><body>

<!--&IF($main,'<!--&MAIN/-->','')-->
<!--&IF($post,'<!--&POST/-->','')-->

</body>
<!--/&READ-->

<!--&POST-->
<form method=post action="{$dir_path}bbs.php">
<input type=hidden name=key value="{$key}">
名前<br>
<input type=text value="{$cname}" name=nick size=20 maxlength=40><br>
E-Mail<br>
<input type=text value="{$cmail}" name=mail size=15><br>
<textarea name=content cols=75 rows=3 class=input></textarea><br>
<input type=password value="{$cpass}" name=delk size=8 maxlength=8><br>
験証<br>
以下の入力ﾌｨｰﾙﾄﾞをｸﾘｱしてください｡<br>
<input type=text value="x" name=url size=8><br>
<input type=submit value="投稿"><input type=reset value="ﾘｾｯﾄ">
</form>
<!--/&POST-->

<!--&MAIN-->
<a href="{$dir_path}i/">戻</a><a href="{$dir_path}read.php?key={$key}&ua=i">全</a><a href="{$dir_path}read.php?key={$key}&ls={$read_def}&ua=i">新</a><a href="#saigo">↓</a><br>
<font size=+1 color="#FF0000">●{$fsub}</font>
<hr>
<!--&IF($first,'<!--&FIRST/-->','')-->
<!--&FOREACH($res,'RES')-->

<a name=saigo></a><hr>
<!--&IF($prev,'<a href="{$dir_path}read.php?key={$key}&st={$prev_st}&to={$prev}&ua=i">前</a>','')-->
<!--&IF($next,'<a href="{$dir_path}read.php?key={$key}&st={$num}&to={$next}&ua=i">次</a>','')-->
<br><a href="{$dir_path}i/">戻</a> <!--&IF($stopped,'','<a href="{$dir_path}read.php?key={$key}&mode=post&ua=i">レス</a>')--><br>
<!--/&MAIN-->

<!--&FIRST-->
[1] <font color="#006600"><!--&IF($femail,'<a href="mailto:{$femail}">{$fname}</a>','{$fname}')--></font> ({$fdate})<br>
{$fcom}
<!--/&FIRST-->

<!--&RESSHOW-->
<hr>[{$num}] <font color="#006600"><!--&IF($email,'<a href="mailto:{$email}">{$name}</a>','{$name}')--></font> ({$date})<br>
{$com}
<!--/&RESSHOW-->

<!--&RES-->
<!--&IF($show,'<!--&RESSHOW/-->','')-->
<!--/&RES-->
