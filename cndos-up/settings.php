<?php
  $title	= '上傳區';	//タイトル
  $home		= 'http://mydomain.com/';	//ホーム
  $logfile	= "up.log";	//ログファイル名（変更する事）
  $updir 	= "./img/";	//アップ用ディレクトリ
  $upsdir	= "./imgs/";	//サムネイル保存ディレクトリ
  $prefix	= '';		//接頭語（up001.txt,up002.jpgならup）
  $logmax	= 5000;		//log保存行（これ以上は古いのから削除）
  $usrmax	= 25;		//User投稿量制限（バイト。全角はこの半分）
  $commax	= 250;		//コメント投稿量制限（バイト。全角はこの半分）
  $limitk	= 100000;	//アップロード制限（KB キロバイト）
  $page_def	= 15;		//一ページの表示行数
  $admin	= "del";	//削除管理パス
  $auto_link	= 1;		//コメントの自動リンク（Yes=1;No=0);
  $denylist	= array('192.168.0.1','sex.com','annony');	//アクセス拒否ホスト
  $arrowext	= array('7z','zip','lzh','rar','avi','mpg','wmv','mp3','wma','swf','txt','bmp','jpg','png','gif');	//許可拡張子 小文字（それ以外はエラー
  $denyupload	= array('annony');	//拒否ファイル
$login_backend	= 'http://mydomain.com/forum/logging_discuz25_up.php';		//コメントの自動リンク（Yes=1;No=0);

  $count_start	= "2008/02/11";	//カウンタ開始日
  $count_file	= "count.txt";  //カウンタファイル（空ファイルで666）

  $last_file	= "last.cgi";	//連続投稿制限用ファイル（空ファイルで666）
  $last_time	= 0;		//同一IPからの連続投稿許可する間隔（分）（0で無制限）

  /* 項目表示（環境設定）の初期状態 (表示ならChecked 表示しないなら空) */
  $f_act  = 'checked="checked"';	//ACT（削除リンク）
  $f_usr  = 'checked="checked"';	//コメント
  $f_com  = 'checked="checked"';	//コメント
  $f_size = 'checked="checked"';	//ファイルサイズ
  $f_mime = '';				//MIMEタイプ
  $f_date = 'checked="checked"';	//日付け
  $f_orig = 'checked="checked"';	//元ファイル名
