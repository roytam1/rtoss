New commands added in lil script runner:
- getenv <EnvVarName> : Get environment varable
- setenv <EnvVarName> <Value> : Set environment varable
- lilpath : Return full path of interpeter executable
- specialdir <CSIDL-id> : Return full path of special folder by ID:
  * 0,16 = Desktop
  * 2 = "Programs" folder of Start Menu
  * 5 = My Documents
  * 6 = Favorites
  * 7 = "Startup" folder of Start Menu
  * 8 = "Recent" folder of Start Menu
  * 9 = Send To
  * 11 = "Start Menu" folder
  * 20 = Fonts
  * 21 = Templates
  * 22 = Common "Start Menu" folder
  * 23 = Common "Programs" folder of Start Menu
  * 24 = Common "Startup" folder of Start Menu
  * 25 = Common "Desktop" folder
  * 26 = ProgramData(Application Data)
  * 27 = PrintHood
  * 31 = Common Favorites
  * 32 = Internet Explorer Cache folder
  * 33 = Internet Explorer Cookies folder
  * 34 = Internet Explorer History folder
- getcwd : Get path of current directory
- chdir <path> : Change directory
