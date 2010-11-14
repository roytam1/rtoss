<?php
  $title	= '上傳區';	//タイトル
  $banner	= '歡迎使用上傳區。';	//バナー
  $home		= 'http://mydomain.com/';	//ホーム
  $sqlite_file	= "k-up.sdb";	//ログファイル名（変更する事）
  $updir 	= "./file/";	//アップ用ディレクトリ
  $prefix	= 'up';		//接頭語（up001.txt,up002.jpgならup）
  $logmax	= 500;		//log保存行（これ以上は古いのから削除）
  $commax	= 250;		//コメント投稿量制限（バイト。全角はこの半分）
  $limitk	= 5000;	//アップロード制限（KB キロバイト）
  $page_def	= 15;		//一ページの表示行数
  $tz	= 8;		// time zone
  $countnumbers	= 6;	// count format number
  $admin	= "del";	//削除管理パス
  $auto_link	= 1;		//コメントの自動リンク（Yes=1;No=0);
  $denylist	= array('annony');	//アクセス拒否ホスト
  $denyuplist	= array('annony');	//うｐ拒否ホスト
  $arrowext	= array('7z','zip','lzh','rar','avi','mpg','wmv','mp3','wma','swf','txt','bmp','jpg','png','gif','psd','aac');	//許可拡張子 小文字（それ以外はエラー
  $denyupload	= array('annony');	//拒否ファイル

  $xsendfile = false; // use x-sendfile header?
                      // false: use PHP readfile_chunked()
                      // 'X-sendfile': for apache mod_xsendfile/lighttpd-1.5
                      // 'X-LIGHTTPD-send-file': for lighttpd-1.4.20+
                      // 'X-Accel-Redirect': for nginx
  $xendfile_prefix = ''; // path prefix for x-sendfile

  $count_start	= "2009/10/30";	//カウンタ開始日

  $last_time	= 0;		//同一IPからの連続投稿許可する間隔（分）（0で無制限）
  $no_same_file	= true;		//同一ファイルの連続投稿許可

  /* 項目表示（環境設定）の初期状態 (表示ならChecked 表示しないなら空) */
  $f_act  = 'checked="checked"';	//ACT（削除リンク）
  $f_dlcnt= 'checked="checked"';	//DL count
  $f_com  = 'checked="checked"';	//コメント
  $f_size = 'checked="checked"';	//ファイルサイズ
  $f_mime = '';				//MIMEタイプ
  $f_date = 'checked="checked"';	//日付け
  $f_dlim = 'checked="checked"';	//DL limit
  $f_orig = 'checked="checked"';	//元ファイル名
