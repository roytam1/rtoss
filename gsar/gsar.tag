file   : gsar.tag
author : Tormod Tjaberg
created: 960801 
updated: 2008-01-21 TT

This file depicts errors, corrections and enhancements in gsar

080121 : 1.21
         . Changed search & replace buffer length from 128 bytes to 256 bytes.
         . Updated GPL license with correct address.
         . Updated the documentation removing some old DOS/Zortech peculiarities.  

070314 : 1.20
         . Fixed a small typo.
         . Finally fixed the LARGE buffer issue.

041207 : 1.12
         . Fixed a bug regarding the use of the LARGE value. The bug
           was introduced in 1.11 when the size of the BMG_Buffer was
           changed to be the same size as LARGE. Searching for -s:x01:x00
           in a 2 byte file filled with 0's would hang gsar. The problem
           is related to the way LARGE is used when determining that
           a match cannot be made. 
        
           Thanks to Mr Qiang Wang for reporting and reproducing this bug.

021107 : 1.11
         . Added support for the MinGW compiler.
         . Linted code and fixed a possible overrun bug.
         . Don't complain about a directory not being a regular file.
         . Updated version number and copyright years.
         . Executable in archive is now a WIN32 exe.
960808 : 1.10 
         . Complete rewrite of search & search and replace code,
           now even faster. Fixed search & search and replace bugs.
         . The -s option must now have the search string directly
           following the argument. This is to make it consistent
           with the -r option.
         . Cleaned up the source, renamed a lot of stuff, removed tabs.
         . Tested with more compilers.
         . Easier to get it to compile under unix
         . Don't attempt to open directories.
         . Enhanced the -B command to display the length of the buffers
           and inform the user if the replacement buffer is empty.
         . Dos executable in archive now has support for response files
           just preceed the file name with '@'.
         . BMG routines use run-time generated table.
             
960120 : . 4 years after gsar was first was released I discovered that 
           searching for :x00:x00 bytes would crash the program on various
           files. Search and replace was even worse. Problem tracked down 
           to the buffer handling. Started rewrite of search &
           search and replace code (at a very leisurly pace)

         . Discovered that under SunOS gsar would seg'fault. Tracked
           down to Suns implementation of realloc. According to the
           man pages realloc(NULL, ...) was illegal, even though ANSI
           allows this.

951018 : 1.07a 
         . Fixed few cosmetic things (never released)

940117 : 1.07  
         . Unix version keeps file mode and ID
         . Corrected the LARGE buffer value. In some cases with long
           patterns a jump would be made outside the buffer.

930328 : 1.06  
         . GetArgs now uses GetOpt, context length can be specified on
           the command line. Setvbuf error is no longer fatal.

930127 : 1.05
         . filter implemented, setvbuf used to speed up disk I/O,
           Corrected obscure bug in BMG search function. In some cases the
           character after the actual end of the buffer (very small file)
           could result in a 'LARGE' jump outside the buffer. The solution
           is to make sure that *(strend+1) != *strend.

         . Corrected bug in BMG_SearchReplace function. If the special case
           described above was true 'n' (number of bytes to write from
           search buffer) would become negative, resulting in a huge file.

921122 : 1.04 
         . implemented signal handling.

920610 : 1.03
         . correct filename is now passed in OneSearchReplace

920404 : 1.02 
         . BMG routines use compile time generated table instead of
           filling it in at run time

920314 : . simplified file buffer read --> 1 argument less, ta' HPV

920226 : . temporary name is now really temporary

920223 : . now able to enter hexadecimal numbers on the command line, ud
           du parameters working.

920203 : . main program working all parsing seems to work


