<?php

/**
 * ************************************************************************\
 * 
 * Paul Gareau - paul@xhawk.net
 * Copyright (C) 2001 - Paul Gareau
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * \************************************************************************
 */

function bb2html($string)
{
	global $imgtag;
	$string = preg_replace("/\[b\](.*?)\[\/b\]/si", "<b>\\1</b>", $string);
	$string = preg_replace("/\[i\](.*?)\[\/i\]/si", "<i>\\1</i>", $string);
	$string = preg_replace("/\[u\](.*?)\[\/u\]/si", "<u>\\1</u>", $string);
	$string = preg_replace("/\[p\](.*?)\[\/p\]/si", "<p>\\1</p>", $string);

	$string = preg_replace("/\[color=(\S+?)\](.*?)\[\/color\]/si",
		"<font color=\"\\1\">\\2</font>", $string);

	$string = preg_replace("/\[s([1-7])\](.*?)\[\/s([1-7])\]/si",
		"<font size=\"\\1\">\\2</font>", $string);

	$string = preg_replace("/\[pre\](.*?)\[\/pre\]/si", "<pre>\\1</pre>", $string);
	$string = preg_replace("/\[quote\](.*?)\[\/quote\]/si", "<blockquote>\\1</blockquote>", $string);
	// ttp:→http:
	$string = preg_replace("/(^|[^=\]h])(ttp:)/si", "\\1http:", $string); 
	// 長いURL省略
	$string = preg_replace("/(^|[^=\]\"])(http|https|ftp)(:\/\/[\!-;\=\?-\~]+)/si", "\\1<a href=\"\\2\\3\" target=_blank>\\2\\3</a>", $string);
	$length = 60;
	preg_match_all("/>(https?:\/\/[\!-;\=\?-\~]+)/si", $string, $reg);
	for($i = 0;$i < count($reg[0]);$i++) {
		$out[$i] = (strlen($reg[0][$i]) > $length) ? substr($reg[0][$i], 0, $length) . "..." : $reg[0][$i];
		$string = str_replace($reg[0][$i], $out[$i], $string);
	} 
	// $string = preg_replace("/(^|[^=\]\/])(www\.[\!-;\=\?-\~]+)/si", "\\1<a href=\"http://\\2\" target=_blank>\\2</a>", $string);
	$string = preg_replace("/\[url\](http|https|ftp)(:\/\/\S+?)\[\/url\]/si",
		"<a href=\"\\1\\2\" target=\"_blank\">\\1\\2</A>", $string);

	$string = preg_replace("/\[url\](\S+?)\[\/url\]/si",
		"<a href=\"http://\\1\" target=\"_blank\">\\1</A>", $string);

	$string = preg_replace("/\[url=(http|https|ftp)(:\/\/\S+?)\](.*?)\[\/url\]/si",
		"<a href=\"\\1\\2\" target=\"_blank\">\\3</A>", $string);

	$string = preg_replace("/\[url=(\S+?)\](\S+?)\[\/url\]/si",
		"<a href=\"http://\\1\" target=\"_blank\">\\2</A>", $string);

	$string = preg_replace("/\[email\](\S+?@\S+?\\.\S+?)\[\/email\]/si",
		"<a href=\"mailto:\\1\">\\1</A>", $string);

	$string = preg_replace("/\[email=(\S+?@\S+?\\.\S+?)\](.*?)\[\/email\]/si",
		"<a href=\"mailto:\\1\">\\2</A>", $string);
	if ($imgtag) {
		$string = preg_replace("#\[img\](([a-z]+?)://([^ \n\r]+?))\[\/img\]#si",
			"<img src=\"\\1\" border=0 alt=\"\\1\">", $string);
	} 
#	$string = str_replace("&amp;heart;", "<font color=red>&#9829;</font>", $string);
#	$string = str_replace("うんこ", "<img src=unko.gif>", $string);

	return $string;
} 

?>