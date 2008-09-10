# タイトル名
$title = "Joyful Note";

# タイトルの文字色
$t_color = "#804040";

# タイトルの文字サイズ
$t_size = '26px';

# 文字色を指定
$tx = "#000000";

# 戻り先のURL (index.htmlなど)
$homepage = "../index.html";

# 最大記事数 (親記事+レス記事も含めた数）
$max = 5000;

# 管理者用マスタパスワード (英数字で８文字以内)
$pass = '0123';

# Key for bypress NGWord check (''=disable, '!' will automatically preppended)
$bpngckey='hoge';

# 返信がつくと親記事をトップへ移動 (0=no 1=yes)
$topsort = 1;

# 強制sageスレ数 (n=0)
$maxage = 10;

# 添付機能を許可する (0=no 1=yes)
$clip = 1;

# 返信にも添付機能を許可する (0=no 1=yes)
$res_clip = 1;

# タイトルにGIF画像を使用する時 (http://から記述)
$t_img = "";
$t_w = 180;	# GIF画像の幅 (ピクセル)
$t_h = 40;	#    〃    高さ (ピクセル)

# ファイルロック形式
# → 0=no 1=symlink関数 2=mkdir関数
$lockkey = 0;

# ロックファイル名
$lockfile = './lock/joyful.lock';

# ミニカウンタの設置
# → 0=no 1=テキスト 2=GIF画像
$counter = 0;

# ミニカウンタの桁数
$mini_fig = 6;

# テキストのとき：ミニカウンタの色
$cnt_color = "#BB0000";

# GIFカウンタのとき：画像までのディレクトリ
# → 最後は必ず / で閉じる
$gif_path = "./img/";
$mini_w = 8;		# 画像の横サイズ
$mini_h = 12;		# 画像の縦サイズ

# 添付データサイズキャッシュファイル
$fscfile = './filesize.cache';

# カウンタファイル
$cntfile = './count.dat';

# スクリプトのURL
$script = './joyful.cgi';

# 入り口ファイル名 (''=HTMLモードオフ)
$htmlfile='index.html';

# ログファイルを指定
# → フルパスで指定する場合は / から記述
$logfile = './joyfullog.cgi';

# キャップファイル
$capfile = './caplist.cgi';

# アップロードディレクトリ
# → パスの最後は / で終わること
# → フルパスだと / から記述する
$imgdir = './img/';

# アップロードディレクトリのＵＲＬパス
# → パスの最後は / で終わること
# → フルパスで指定する場合は http:// から記述
$imgurl = "./img/";

# 添付ファイルのアップロードに失敗したとき
#   0 : 添付ファイルは無視し、記事は受理する
#   1 : エラー表示して処理を中断する
$clip_err = 1;

# 記事 [タイトル] 部の長さ (0=関係ない、全角文字換算)
$sub_len = 15;

# メールアドレスの入力必須 (0=no 1=yes)
$in_email = 0;

# トリップ生成機能 (0=使用しない 1=使用する)
$gentrip = 1;

# sage/age機能 (0=使用しない 1=使用する)
$sagemode = 1;

# IDを作成機能 (0=使用しない 1=ID隠し機能を使用する 2=使用する)
$genid = 2;

# 同一IPアドレスからの連続投稿時間（秒数）
# → 連続投稿などの荒らし対策
# → 値を 0 にするとこの機能は無効になります
$wait = 0;

# Allow users delete/modify postes without password and verify by IP only? (0-no, 1-yes)
$modnopass=1;

#Timezone
$TZ="CST-8";

# １ページ当たりの記事表示数 (親記事)
$p_log = 8;

# 投稿があるとメール通知する (sendmail必須)
#  0 : 通知しない
#  1 : 通知するが、自分の投稿記事はメールしない。
#  2 : 通知する。自分の投稿記事も通知する。
$mailing = 0;

# メールアドレス(メール通知する時)
$mailto = 'xxx@xxx.xxx';

# sendmailパス（メール通知する時）
$sendmail = '/usr/lib/sendmail';

# 他サイトから投稿排除時に指定 (http://から書く)
$base_url = "";

#Disable Colors (0=no 1=yes)
$nocolor = 0;

#GazouBBS-liked style (0=no 1=yes)
$template = 'joyfulnote.tpl';

#Anonymous Posting  (0=no 1=yes)
$anonymouspost = 1;

# 1スレッドのレス表示数（-1で機能OFF）
$showres = 6;

#スレッドタイトル表示数（0で機能OFF）
$th_log = 0;

# 文字色の設定（半角スペースで区切る）
$colors = '#800000 #DF0000 #008040 #0000FF #C100C1 #FF80C0 #FF8040 #000080';

# URLの自動リンク (0=no 1=yes)
$autolink = 1;

# タグ広告挿入オプション
# → <!-- 上部 --> <!-- 下部 --> の代わりに「広告タグ」を挿入する。
# → 広告タグ以外に、MIDIタグ や LimeCounter等のタグにも使用可能です。
$banner1 = '<!-- 上部 -->';	# 掲示板上部に挿入
$banner2 = '<!-- 下部 -->';	# 掲示板下部に挿入

# アクセス制限（半角スペースで区切る）
#  → 拒否するホスト名又はIPアドレスを記述（アスタリスク可）
#  → 記述例 $deny = '*.anonymizer.com *.denyhost.xx.jp 211.154.120.*';
$deny = '';

# Block on posting event only? (0=no 1=yes)
$onlyblockonpost=1;

# Query bad IP from external IP database (0=disabled, 1-7=levels, larger=query more but slower)
# Level 1: DSBL チェック（オープンプロクシなどのホストが、登録されています。）
# Level 2: BBX チェック（２ちゃんねるで広告爆撃したホストなどが、登録されています。）
# Level 3: DNSbl チェック（オープンプロクシなどのホストが、登録されています。）
# Level 4: BBQ チェック（２ちゃんねるで悪用されたホストなどが、登録されています。）
# Level 5: RBL チェック（ウイルスメールを発信したホストが、登録されています。）
# Level 6: IRCbl チェック（IRCで爆撃したホストが、登録されています。）
# Level 7: ToR チェック（ SSL オープンプロクシみたいなの。）
$extIPQ=4;

# NGワード（半角コンマで区切る）
$NGWords='';

# ホスト取得方法
# 0 : gethostbyaddr関数を使わない
# 1 : gethostbyaddr関数を使う
$gethostbyaddr = 0;

# アップロードを許可するファイル形式
#  0:no  1:yes
$gif   = 1;	# GIFファイル
$jpeg  = 1;	# JPEGファイル
$png   = 1;	# PNGファイル
$text  = 0;	# TEXTファイル
$lha   = 0;	# LHAファイル
$zip   = 0;	# ZIPファイル
$pdf   = 0;	# PDFファイル
$midi  = 0;	# MIDIファイル
$word  = 0;	# WORDファイル
$excel = 0;	# EXCELファイル
$ppt   = 0;	# POWERPOINTファイル
$ram   = 0;	# RAMファイル
$rm    = 0;	# RMファイル
$mpeg  = 0;	# MPEGファイル
$mp3   = 0;	# MP3ファイル

# 投稿受理最大サイズ (bytes)
# → 例 : 102400 = 100KB
$maxsize = "512000";

# Limits the attachment's total size?
$storage_limit=1;

# Storage Amount for the attachments (in KB)
$storage_amount=10240;

# 画像ファイルの最大表示の大きさ（単位：ピクセル）
# → これを超える画像は縮小表示します
$MaxW = 250;	# 横幅
$MaxH = 250;	# 縦幅

# Show Image width and height? (0=no 1=yes)
$ShowWH=1;

# Thumbnail Quality (1~100)
$QL=75;

# 家アイコンの使用 (0=no 1=yes)
$home_icon = 0;

# アイコン画像ファイル名 (ファイル名のみ)
$IconHome = "home.gif";  # ホーム
$IconClip = "clip.gif";  # クリップ
$IconSoon = "soon.gif";  # COMINIG SOON

# 画像管理者チェック機能 (0=no 1=yes)
# → アップロード「画像」は管理者がチェックしないと表示されない機能です
# → チェックされるまで「画像」は「COMMING SOON」のアイコンが表示されます
$ImageCheck = 0;

# 投稿後の処理
#  → 掲示板自身のURLを記述しておくと、投稿後リロードします
#  → ブラウザを再読み込みしても二重投稿されない措置。
#  → Locationヘッダの使用可能なサーバのみ
$location = '';

# Redirect?
$redirection=1;

# Field trap
@traps = ('name',  'email', 'url', 'sub', 'comment');

# Input field replacement
%fields = ( name => 'nick', email => 'mail', url => 'site', sub => 'title', comment => 'content' );

#---(以下は「過去ログ」機能を使用する場合の設定です)---#
#
# 過去ログ生成 (0=no 1=yes)
$pastkey = 0;

# 過去ログ用NOファイル
$nofile  = './pastno.dat';

# 過去ログのディレクトリ
# → フルパスなら / から記述（http://からではない）
# → 最後は必ず / で閉じる
$pastdir = './past/';

# 過去ログ１ファイルの行数
# → この行数を超えると次ページを自動生成します
$log_line = 600;

#Board Note
$board_note="";

#--- ちびぼーど風の設定 --------------------------------#
$cb_neme = 'ちびぼーど風 VIEW';
$cb_ver = 'ver1.1';

# 初期表示コメント(紹介文等)
$top_mes = '気軽に書き込んで下さい(^.^)';

# Joyfulを開くターゲットウインドウ名
$target_win = "_top";


# 読み込むメッセージ件数
$look_cnt = 20;

# 表示間隔(秒)
$itv_time = 5;

# 表示スピード％ (1～100)
$speed = 4;

# 文字の流れ込む方向 (left:左から右へ right:右から左へ)
$scroll = 'right';

#情報のオートリロード時間(秒) リロードしない場合は0
$data_re_load_time = 300;

# ○時間以内の記事にNEWを表示（使わない時は0）
$newh = 72;

# 表示するコメントの文字数
$msg_len = 32;

# NEWに使う画像
$newga = "new.gif";

# リロードアイコンのファイル名
$reload_gif = "reload.gif";

# BBSアイコンのファイル名
$bbs_gif = "memo.gif";
