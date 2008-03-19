<!--&header--><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=UTF-8">
<META HTTP-EQUIV="pragma" CONTENT="no-cache">
<META HTTP-EQUIV="expires" CONTENT="Thu, 1 Jan 1970 00:00:00 GMT">
<STYLE type="text/css">
<!--
html { background: #FFE; color: #800000; } /* 基本網頁背景樣式 */
a:link { color: #00E; } /* 正常連結樣式 */
a:hover { color: #D00; } /* hover時連結樣式 */
a:visited { color: #00E; } /* 已拜訪連結樣式 */
small { font-size: 0.8em; } /* 小字樣式(eg.[以預覽圖顯示]) */
body { font-size:12pt; font-family: "ＭＳ Ｐゴシック" ,"Osaka",sans-serif;}
blockquote {margin: 0.8em 2em 0.8em 2em;}
.admnote, .admnote * { margin-left: 0px;text-indent: 0px;list-style-position:inside;padding-left:0px;}
.nor {text-decoration: none;font-weight:normal;}
.name {color:#080;font-weight:bold;}
.title {color:#b44;font-weight:bold;}
.warn_txt {color:#888;}
.warn_txt2 {color:#f00;font-weight:bold;}
.quoted {color:#777;}
.res {padding-left:1.5em;}
.img {float:left; margin:0.3em;}
form {margin:0px;}
.h {display:none;}
/* For Infoseek */
iframe {width:0px;height:0px;display:none;}
-->
</STYLE>
<script type="text/javascript">
<!--
function l(e){var J=getCookie("JoyfulNote"),i,jn;if(J=="")return true;jn=J.split("<>");if(jn.length!=7)return true;with(document){for(i=0;i<forms.length;i++){with(forms[i]){if(pwd && pwd.type!="hidden")pwd.value=jn[3];if({$fields_name}){$fields_name}.value=jn[0];if({$fields_url}){$fields_url}.value=jn[2];if({$fields_email}){$fields_email}.value=jn[1].replace("+"," ");{$jscol}}}}return true;}onload=l;
function getCookie(key, tmp1,tmp2,xx,xx1,xx2,xx3,len) {if(!document.cookie)return "";tmp1 = " "+document.cookie+";";if(tmp1.indexOf(key)==-1) return "";xx1=xx2=0;len=tmp1.length;while(xx1<len){xx2=tmp1.indexOf(";",xx1);tmp2=tmp1.substring(xx1+1,xx2);xx3=tmp2.indexOf("=");if(tmp2.substring(0,xx3)==key){return decodeURL(tmp2.substring(xx3+1,xx2-xx1-1));}xx=xx2+1;}return "";}
function decodeURL(str){var s0='',i,j,s,ss,u,n,f,sss;for(i=0;i<str.length;i++){s=str.charAt(i);if(s=="+"){s0+=" ";}else{if(s!="%"){s0+=s;}else{u=0;f=1;while(true){ss='';for(j=0;j<2;j++){sss=str.charAt(++i);if(((sss>="0")&&(sss<="9"))||((sss>="a")&&(sss<="f"))||((sss>="A")&&(sss<="F"))){ss+=sss;}else{--i;break;}}n=parseInt(ss,16);if(n<=0x7f){u=n;f=1;}if((n>=0xc0)&&(n<=0xdf)){u=n&0x1f;f=2;}if((n>=0xe0)&&(n<=0xef)){u=n&0x0f;f=3;}if((n>=0xf0)&&(n<=0xf7)){u=n&0x07;f=4;}if((n>=0x80)&&(n<=0xbf)){u=(u<<6)+(n&0x3f);--f;}if(f<=1){break;}if(str.charAt(i+1)=="%"){i++;}else{break;}}s0+=String.fromCharCode(u);}}}return s0;}
-->
</script>
{$meta}
<title>{$title}</title></head>
<body>
<!--/&header-->

<!--&form-->
<!--&IF($newmode,'<center>','')-->
<!--&IF($resmode,'<hr>','')-->
<form action="{$script}?{$langstr}" method="POST" {$clip}>
{$hiddens}
<table border=0 cellspacing=0>
<tr>
  <td nowrap><b>{$jnstr_name}</b></td>
  <td>{$jn_name_field}</td>
</tr>
<tr>
  <td nowrap><b>{$jnstr_email}</b></td>
  <td>{$jn_email_field}</td>
</tr>
<tr>
  <td nowrap><b>{$jnstr_title}</b></td>
  <td nowrap>{$jn_sub_field}{$jn_submit_button}{$jn_reset_button}
  </td>
</tr>
<tr>
  <td colspan=2>
    <b>{$jnstr_commets}</b><br>
    {$jn_comment_field}
  </td>
</tr>
<tr>
  <td nowrap><b>URL</b></td>
  <td>{$jn_url_field}</td>
</tr>
<!--&IF($jnstr_attachment,'<!--&form_up/-->','')-->
<!--&IF($jnstr_delkey,'<!--&form_pass/-->','')-->
<tr><td nowrap>
<!--&IF($jn_color,'<b>{$jnstr_txtcol}</b></td><td>','')-->
<!--&FOREACH($jn_color,'form_color')-->
</td></tr>
<tr><td colspan=2><ul class='admnote'>
{$jn_notes}
</ul></td></tr>
</table></form>
<!--&IF($newmode,'</center>','')-->
<!--&IF($resmode,'<hr>','')-->
<!--/&form-->

<!--&form_up-->
<tr><td><b>{$jnstr_attachment}</b></td>
<td>{$jnstr_attachment_field}</td></tr>
<!--/&form_up-->

<!--&form_pass-->
<tr><td nowrap><b>{$jnstr_delkey}</b></td>
<td>{$jnstr_delkey_field}
{$jnstr_delkey_post}</td></tr>
<!--/&form_pass-->

<!--&form_color-->
{$color_button}<font color="{$color}">■</font>
<!--/&form_color-->

<!--&threads-->
<!--&FOREACH($th,'thread')-->
<!--/&threads-->

<!--&thread-->
<!--&IF($resmode,'','<center><table border=1 valign=top width=90% bgcolor="#FFFFFF"><tr><td>')-->
<span class="title">{$SUB}</span> {$NAME_TEXT}<span class="name">{$NAME}</span> {$TIME_TEXT} {$TIME} No.{$NO}&nbsp;{$REPLYBTN}{$PROCBTN}
<div class="container">{$ATTECH}<blockquote style="color:{$COL}">{$COM}</blockquote>{$WARN_BEKILL}{$WARN_HIDEPOST}</div>
<!--&FOREACH($res,'res')-->
<!--&IF($resmode,'','</td></tr></table></center>')-->
<br>
<!--/&thread-->

<!--&res-->
<!--&IF($resmode,'','<hr width=90%>')-->
<div class="res">
<span class="title">{$SUB}</span> {$NAME_TEXT}<span class="name">{$NAME}</span> {$TIME_TEXT} {$TIME} No.{$NO}&nbsp;{$PROCBTN}<br/>
<div class="container">{$ATTECH}<blockquote style="color:{$COL}">{$COM}</blockquote>{$WARN_BEKILL}</div>
</div>
<!--/&res-->

<!--&resform-->
{$THREAD}
{$FORM}
<!--/&resform-->

<!--&searchresults-->
<hr>
<!--&FOREACH($p,'searchresult')-->
<!--/&searchresults-->

<!--&searchresult-->
<div class="threadpost">
<span class="title">{$SUB}</span> {$NAME_TEXT}<span class="name">{$NAME}</span> {$TIME_TEXT} {$TIME} No.{$NO}{$RENO}&nbsp;{$PROCBTN}
<div class="container">{$ATTECH}<blockquote style="color:{$COL}">{$COM}</blockquote>{$WARN_BEKILL}</div>
</div>
<hr>
<!--/&searchresult-->

<!--&chibi--><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>
<head>

  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
  <meta http-equiv="Content-Style-Type" content="text/css" />
  <meta name="robots" content="index,follow" />
{$reload}

<title>{$title}</title>

<style>
<!--
body {
  font-family: "ＭＳ Ｐゴシック" ,"Osaka",sans-serif;
  margin: 0px;
  margin-top: 2px;
  margin-bottom: 2px;
  background: none;
}

a {
  color: #7b68ee;
  text-decoration: none;
  background: none;
  border-bottom: 1px dotted #eeeeee;
}
a:link {
  color: #7b68ee;
  text-decoration: none;
  background: none;
  border-bottom: 1px dotted #eeeeee;
}
a:link:hover {
  color: #d71345;
  text-decoration: none;
  background: none;
  border-bottom: 1px solid #d71345;
}
a:link:active {
  color: #d71345;
  text-decoration: none;
  background: none;
  border-bottom: 1px solid #d71345;
}

img {
  border: 0px;
  margin: 1px;
}

.icon {
  border: 0px;
  margin: 1px;
  float: left;
}

.com {
  height: 35px;
  padding-top: 3px;
  text-align: left;
  text-decoration: none;
  font-size: 13px;
  font-weight: normal;
  border: 0px;
  border-bottom: 1px solid;
}

.com h1 {
  margin: 0px;
  margin-left: 2px;
  padding: 0px;
  color: green;
  font-size: 14px;
  font-style: normal;
}

.com em {
  color: red;
  font-size: 14px;
  font-style: normal;
}

.com small {
  color: gray;
  font-size: 11px;
}

.com p {
  margin: 0px;
  padding: 0px;
  padding-left: 5px;
  font-size: 13px;
}

#admin {
  margin: 0px;
  padding: 0px;
  padding-right: 3px;
  font-size: 11px;
  text-align: right;
}
/* For Infoseek */
iframe {display:none;}

-->
</style>

<script language="JavaScript">
<!--
var interval = {$itv_time};

interval *= 1000;

var w = 0;
var num = 0;
var id = "mes" + num;

function view() {

  if (!document.getElementById(id)) {
    num = 0;
    id = "mes" + num;
  }

  w = w + {$speed};

  if (w <= 100 + {$speed}) {
    if (w > 100) {
      document.getElementById(id).style.width = '100%';
    } else {
      document.getElementById(id).style.width = w + '%';
    }
    document.getElementById(id).style.display = "block";

  } else {
    w = 0;
    document.getElementById(id).style.display = "none";
    num ++;
    id = "mes" + num;
  }

  if (w < 100) {
    setTimeout ("view()",10);

  } else {
    setTimeout ("view()",interval);
  }

}

// -->
</script>

</head>
<body style="text-align: {$scroll};" onload="view();">
<!--&IF($help,'<!--&chibiHelp/-->','<!--&chibiMain/-->')-->
<!--/&chibi-->

<!--&chibiMain-->
<div id="msg_box">
  <div id="mes0" class="com" style="display: none; white-space: nowrap; overflow: hidden;">
    <h1>{$title}</h1>
    <p>{$top_mes}</p>
  </div>
<!--&FOREACH($mes,'chibiMesg')-->
</div>

<div id="admin">
  <a href="{$script}" target="{$target_win}">{$jnstr_chibi_gotobbs}</a>
  <a href="JavaScript:location.reload();">{$jnstr_reload}</a>
  <a href="{$script}?mode=chibihelp" target="_self">{$cb_neme}</a>
</div>

</body>
</html>
<!--/&chibiMain-->

<!--&chibiMesg-->
  <div id="mes{$i}" class="com" style="display: none; white-space: nowrap; overflow: hidden; border-color: #000;">
    <em>{$newimg}</em>
     {$sub} <small>[{$name}{$url}] {$date} ({$no}, re: {$reno})</small>
    <p style="color: {$color};">{$comment}</p>
  </div>
<!--/&chibiMesg-->

<!--&chibiHelp-->
<div id="msg_box">
<!--&FOREACH($mes,'chibiHelpMesg')-->
</div>

<div id="admin">
  <a href="{$script}?mode=chibi&amp;{$langstr}" target="_self">{$jnstr_back}</a>
  <a href="{$dest}" target="{$target_win}">{$jnstr_chibi_gotobbs}</a>
  presented by <a href="http://homepage2.nifty.com/hary/" target="_blank">@Hary</a>
</div>

</body>
</html>
<!--/&chibiHelp-->

<!--&chibiHelpMesg-->
  <div id="mes{$i}" class="com" style="display: none; white-space: nowrap; overflow: hidden; border-color: {$bgcolor};">
{$mesg}
  </div>
<!--/&chibiHelpMesg-->
