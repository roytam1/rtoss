23:00 2004/08/05
suga@snpn.net

repng2jpeg 1.0.4

jpeg,png,gif => jpeg,png,gif のフォーマットおよびサイズ変更

あんまり使い道も無いと思っていたのですが
それなりに使う人がいるっぽいのでまとめておいておきます

同梱してあるバイナリはコンパイル環境に近ければ
そのまま動く可能性もあるというおまけ程度なので
動かなかったらあきらめることも必要です(ぉ

お約束事ですがこのアーカイブに含まれている
バイナリ/ソースは各自の責任においてご利用下さい

OSによっていくつか用意してみました
dynamic   : libjpeg libpng の共有ライブラリを必要とするもの ファイルサイズ小
standard  : libjpeg libpng はstaticにしてあるもの libcあたりのバージョンが同じなら動くはず
static    : スタティックバイナリ  ex.linux版はlinuxカーネルが動いていれば動くかも

下に行くほど使う共有ライブラリが少ないので動く可能性が高いです、
そのかわりファイルサイズが大きくなります。

---
コマンドの使い方

$repng2jpeg inputfile outputfile (width height|Z ratio) [Quality|P|G]

inputfile   -> 入力ファイル名 (JPEG,PNG,GIF)
outputfile  -> 出力ファイル名 (JPEG,PNG,GIF)

width       -> リサイズ後の横px
height      -> リサイズ後の縦px

縦横比を保存したまま拡大縮小する場合は
第三引数に z または Z を指定し
第四引数に 倍率を指定します

[option]
JPEG書き込みの場合 JPEGクオリティ値(1-100)を
PNG書き込みの場合 P 又は p ,
GIF書き込みの場合 G 又は g を指定します
省略された場合は 75 相当が補完されます

ex.
$repng2jpeg 1.jpg 2.jpg 400 300    # 400x300にリサイズしJPEG(75)で書き込み
$repng2jpeg 1.jpg 2.jpg 400 300 90 # 400x300にリサイズしJPEG(90)で書き込み
$repng2jpeg 1.jpg 2.png 400 300 P  # 400x300にリサイズしPNGで書き込み
$repng2jpeg 1.jpg 2.gif Z 0.5 G    # 0.5倍にリサイズしGIFで書き込み

---
依存関係というか配布版バイナリの使用するライブラリ群
static版は特に共有ライブラリを必要としません。

i386_freebsd4_dynamic
libjpeg -> libjpeg.so.9
libpng  -> libpng.so.5
libz    -> libz.so.2
libm    -> libm.so.2
libc    -> libc.so.4

i386_freebsd4_standard
libz    -> libz.so.2
libm    -> libm.so.2
libc    -> libc.so.4

i386_linux_dynamic
libjpeg -> libjpeg.so.62 
libpng  -> libpng.so.2
libz    -> libz.so.1
libm    -> libm.so.6
libc    -> libc.so.6
ld-linux.so.2

i386_linux_standard
libz    -> libz.so.1
libm    -> libm.so.6
libc    -> libc.so.6
ld-linux.so.2

i386_solaris_standard
libc    -> libc.so.1
libdl   -> libdl.so.1

sparc_solaris_standard
libc    -> libc.so.1
libdl   -> libdl.so.1
libc_psr.so

i386_win32
 -> msvcrt.dll

--
i386_freebsd4_dynamic    SIZE: 59036　/ MD5: a375f3fc944740ca5706f0ea272fb5c0
i386_freebsd4_standard   SIZE: 254296 / MD5: ee64f97139cdce1222ec41aa79e32613
i386_freebsd4_static     SIZE: 371324 / MD5: 0ff9a08698c08b3898a50f6855cf1ab5
i386_linux_dynamic       SIZE: 59556  / MD5: 6f03c0ec02237299cd8f9a9e2212195a
i386_linux_standard      SIZE: 252468 / MD5: bf4db47cf12f43939892444da0d1ec98
i386_linux_static        SIZE: 661768 / MD5: 63e917c291e4476b0719756dd2df595b
i386_solaris_standard    SIZE: 332272 / MD5: 34091fe828658575ef5bdf9e21949816
i386_solaris_static      SIZE: 501804 / MD5: 0647c706ab30dc16779e71b3c7b7ae87
sparc_solaris_standard   SIZE: 416324 / MD5: d43ce38b0033ff474ec28c9df159c881
sparc_solaris_static     SIZE: 643624 / MD5: 27c0e47a8a439126212411d3c8fd0785
i386_win32               SIZE: 153600 / MD5: 043b3c41dd749365b960f1a7320a69ed


--
コンパイル環境
Linux   -> Vinelinux2.6r4(i386)
FreeBSD -> FreeBSD4.10R(i386)
Solaris -> SunOS5.8(i386/sparc)
Windows -> Windows2000SP4 + MinGW

---
1.0.3 -> 1.0.4 GIF書き込み、倍率オプション追加
1.0.2 -> 1.0.3 GIF読み込みサポート
1.0.1 -> 1.0.2 PNG書き込みサポート
1.0.0 -> 1.0.1 JPEG読み込みサポート
1.0.0 png→jpegのサイズ及びフォーマットコンバータとして作成
