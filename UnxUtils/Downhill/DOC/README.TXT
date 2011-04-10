

                              THE DOWNHILL PROJECT
                         Porting UNIX to Microsoft Win32

                                  Release 1.3


ADDRESS
	The Downhill Project can be reached at knauss@netcom.com.  If
	you're feeling helpful, put "DOWNHILL" in the subject.


PURPOSE
	The Downhill Project is a collection of Win32-compatible
	routines designed to emulate UNIX API calls.  These routines
	allow UNIX code to run under Win32 OS's (Windows 95, Windows
	NT and Windows 3.1 with Win32s) with as little modification as
	possible, without a lot of ugly #ifdefs.


UNIVERSALITY
	The Downhill code, at this point, is written for me.  It got
	the job done that I needed it to do, but it may not work as
	well for you.  Your mileage _will_ vary.

	I am interested, however, in making it as universal as
	possible.  I suppose a blue-sky goal of this project would be
	to completely emulate as many different UNIXen as possible.

	So if you make any changes in the code to get it to work under
	your UNIX/Win32 combination, please let me know.  I'll be
	making regular releases of everything I've collected and you
	will be credited for your contributions.

	Please, send me everything you do!  The success of this project
	largely depends on its universality.  Thanks.


NAMING CONVENTION
	It's called FNORD and the fact that I've documented it proves
	that I have far too much time on my hands.  If you're curious,
	write me and I'll send you the paper.


INSTALLATION
	Untar the Downhill directory tree someplace where it will be
	out of the way ("\DOWNHILL", for example).  Change the makefile
	variables to point the appropriate places or values:
		
		CMP_SYSTEM_DIR	Where the system include files are
		CMP_POSIX	Compile POSIX-compliant functions only?
		CMP_DEBUG	Turn on debugging for library?

	Add the include directory ("\DOWNHILL\INCLUDE") to your
	compiler's include path.  Add the library
	("\DOWNHILL\DOWNHILL.LIB") to your linker's library list.

	After Downhill is installed, when your compiler isn't able to
	resolve a reference (say, "#include <sys/param.h>"), it will
	continue to check through any additional include directories,
	eventually stumbling across the Downhill Project, where it will
	(hopefully) find the file it is looking for.  This will then be
	included as normal and things will continue on their way.

	If you're planning on using any of the Downhill-specific
	functions (or want a single include that does all the foo()-
	to-_foo() work), include "downhill.h".  You should never have
	to inlcude anything in the "DOWNHILL\INCLUDE\DOWNHILL"
	directory -- it's all internal stuff.

	Note: You don't need to change your #include <>'s to
	#include ""'s.  The compiler will search the additional paths
	anyway.


DISCLAIMER
	Nothing you do with this stuff is my fault.  You're welcome to
	any of it, but if you screw something up, it's none of my
	doing.  Please don't sue me.

	UNIX is a trademark of...  Ah, who the hell knows these days?
	Windows, Windows NT, Win32, Win32s and just about everything
	 else in the industry is a trademark of Microsoft.
