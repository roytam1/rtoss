=========== PokemonHackSourceCode Readme ===========
Author: Fuzzier
EMail:  Gauchyler@sina.com

================= Project List =====================
ProcessHandler
	功能：关联进程，提供函数读写进程内存空间。
	类型：静态库。
	依赖：无。

VisualBoyHandler
	功能：关联VisualBoyAdvance进程，提供函数读写VBA模拟的GBA内存空间。
	类型：静态库。
	依赖：ProcessHandler

PokemonCodec
	功能：
		PokemonTool：提供函数将日文/英文字符串转换为游戏内部代码。
		PokemonCodec：提供CPokemonCodec类以修改宠物的内存数据。
	类型：静态库。
	依赖：MFC库。

PokemonRom
	功能：提供CPokemonRom类从ROM文件或者VBA内存读取ROM信息。
	类型：静态库。
	依赖：VisualBoyHandler，MFC库。

PokemonMemHack
	功能：
		提供CPokemonMemHackCore类修改非宠物信息（如物品等）。
		提供用户界面修改宠物游戏。
	类型：EXE可执行文件。
	依赖：PokemonRom，PokemonCodec，MFC库。

PokemonRomViewer
	功能：提供用户界面修改宠物ROM。
	类型：EXE可执行文件。
	依赖：PokemonRom，MFC库。
