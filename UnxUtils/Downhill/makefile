# *****************************************************************************
# *                                                                           *
# * MAKEFILE                                                                  *
# *                                                                           *
# * Freely redistributable and modifiable.  Use at your own risk.             *
# *                                                                           *
# * Copyright 1994 The Downhill Project                                       *
# *                                                                           *
# *****************************************************************************


# User controlled settings ****************************************************

# Where things are ============================================================
CMP_SYSTEM_DIR      =  C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE   # Where the system includes are

# How things are compiled =====================================================
CMP_POSIX           = NO                   # Compile as POSIX-compliant only?
CMP_SYSV            = YES                  # Compile as System V-compliant?
CMP_DEBUG           = NO                   # Turn on debugging for compiles?

# How things work =============================================================
DWN_MACHINE_TYPE      = x86                # Type of machine
DWN_GETOPT_SWITCH     = /                  # Command-line switch, also "-"
DWN_HOSTSEQUIV_PATH   = C:\\ETC\\HOSTS.EQV # Location of /etc/hosts.equiv
DWN_RHOSTS_FILE       = RHOSTS             # Filename for .rhosts
DWN_NETRC_FILE        = NETRC              # Filename for .netrc
DWN_STAT_BLOCKSIZE    = 512                # Device block size
DWN_USER_NAME         = user               # User's name
DWN_USER_IDREAL       = 0                  # User's real ID
DWN_USER_IDEFFECTIVE  = 0                  # User's effective ID
DWN_USER_IDSAVED      = 0                  # User's saved ID
DWN_USER_PASSWORD     = *                  # User's encrypted password
DWN_USER_GECOS        = User               # User's GECOS information
DWN_USER_DIR          = C:\\              
DWN_USER_SHELL        = C:\\windows\\system32\\CMD.exe
DWN_GROUP_NAME        = group              # User's group's name
DWN_GROUP_IDREAL      = 0                  # User's real group ID
DWN_GROUP_IDEFFECTIVE = 0                  # User's effective group ID
DWN_GROUP_IDSAVED     = 0                  # User's saved group ID
DWN_GROUP_PASSWORD    = *                  # User's group's encrypted password


# Things the user should keep his mits off of *********************************

# Targets and objects =========================================================
LIB_TARGET         = DOWNHILL.LIB
LIB_SRCS           = SRC\DH_DIR.C   \
		      SRC\DH_FILE.C \
		      SRC\DH_GRP.C  \
		      SRC\DH_MEM.C  \
		      SRC\DH_MISC.C \
		      SRC\DH_NET.C  \
		      SRC\DH_PROC.C \
		      SRC\DH_SIG.C  \
		      SRC\DH_STR.C  \
		      SRC\DH_TIME.C \
		      SRC\DH_USR.C
LIB_OBJS           = $(LIB_SRCS:.C=.OBJ)
INC_SYSTEM_DIR     = INCLUDE\DOWNHILL\DH_INC
INC_POSIX_DEFINE   = INCLUDE\DOWNHILL\DH_POSIX.H

# Compiler stuff ==============================================================
CMP_FLAGS_PATH     = /I"INCLUDE"
!IF "$(CMP_DEBUG)" == "YES"
CMP_FLAGS_DEBUG    = /Zi /Od
!ELSE
CMP_FLAGS_DEBUG    = /Ox
!ENDIF
!IF "$(CMP_SYSV)" == "YES"
CMP_FLAGS_UNIX     = /D"_DOWNHILL_UNIX_SYSV"
!ELSE
CMP_FLAGS_UNIX     =
!ENDIF
!IF "$(DWN_MACHINE_TYPE)" != ""
CMP_FLAGS_MACHINE_TYPE = /D"_DOWNHILL_MACHINE_TYPE=\"$(DWN_MACHINE_TYPE)\""
!ELSE
CMP_FLAGS_MACHINE_TYPE =
!ENDIF
!IF "$(DWN_GETOPT_SWITCH)" != ""
CMP_FLAGS_GETOPT_SWITCH = /D"_DOWNHILL_GETOPT_SWITCH='$(DWN_GETOPT_SWITCH)'"
!ELSE
CMP_FLAGS_GETOPT_SWITCH =
!ENDIF
!IF "$(DWN_STAT_BLOCKSIZE)" != ""
CMP_FLAGS_STAT_BLOCKSIZE = /D"_DOWNHILL_STAT_BLOCKSIZE=$(DWN_STAT_BLOCKSIZE)"
!ELSE
CMP_FLAGS_STAT_BLOCKSIZE =
!ENDIF
!IF "$(DWN_HOSTSEQUIV_FILE)" != ""
CMP_FLAGS_HOSTSEQUIV_FILE = /D"_DOWNHILL_HOSTSEQUIV_FILE= \
			     \"$(DWN_HOSTSEQUIV_FILE)\""
!ELSE
CMP_FLAGS_HOSTSEQUIV_FILE =
!ENDIF
!IF "$(DWN_RHOSTS_FILE)" != ""
CMP_FLAGS_RHOSTS_FILE = /D"_DOWNHILL_RHOSTS_FILE=\"$(DWN_RHOSTS_FILE)\""
!ELSE
CMP_FLAGS_RHOSTS_FILE =
!ENDIF
!IF "$(DWN_NETRC_FILE)" != ""
CMP_FLAGS_NETRC_FILE = /D"_DOWNHILL_NETRC_FILE=\"$(DWN_NETRC_FILE)\""
!ELSE
CMP_FLAGS_NETRC_FILE =
!ENDIF
!IF "$(DWN_USER_NAME)" != ""
CMP_FLAGS_USER_NAME = /D"_DOWNHILL_USER_NAME=\"$(DWN_USER_NAME)\""
!ELSE
CMP_FLAGS_USER_NAME =
!ENDIF
!IF "$(DWN_USER_IDREAL)" != ""
CMP_FLAGS_USER_IDREAL = /D"_DOWNHILL_USER_IDREAL=$(DWN_USER_IDREAL)"
!ELSE
CMP_FLAGS_USER_IDREAL =
!ENDIF
!IF "$(DWN_USER_IDEFFECTIVE)" != ""
CMP_FLAGS_USER_IDEFFECTIVE = /D"_DOWNHILL_USER_IDEFFECTIVE= \
			      $(DWN_USER_IDEFFECTIVE)"
!ELSE
CMP_FLAGS_USER_IDEFFECTIVE =
!ENDIF
!IF "$(DWN_USER_IDSAVED)" != ""
CMP_FLAGS_USER_IDSAVED = /D"_DOWNHILL_USER_IDSAVED=$(DWN_USER_IDSAVED)"
!ELSE
CMP_FLAGS_USER_IDSAVED =
!ENDIF
!IF "$(DWN_USER_PASSWORD)" != ""
CMP_FLAGS_USER_PASSWORD = /D"_DOWNHILL_USER_PASSWORD= \
			   \"$(DWN_USER_PASSWORD)\""
!ELSE
CMP_FLAGS_USER_PASSWORD =
!ENDIF
!IF "$(DWN_USER_GECOS)" != ""
CMP_FLAGS_USER_GECOS = /D"_DOWNHILL_USER_GECOS=\"$(DWN_USER_GECOS)\""
!ELSE
CMP_FLAGS_USER_GECOS =
!ENDIF
!IF "$(DWN_USER_DIR)" != ""
CMP_FLAGS_USER_DIR = /D"_DOWNHILL_USER_DIR=\"$(DWN_USER_DIR)\""
!ELSE
CMP_FLAGS_USER_DIR =
!ENDIF
!IF "$(DWN_USER_SHELL)" != ""
CMP_FLAGS_USER_SHELL = /D"_DOWNHILL_USER_SHELL=\"$(DWN_USER_SHELL)\""
!ELSE
CMP_FLAGS_USER_SHELL =
!ENDIF
!IF "$(DWN_GROUP_NAME)" != ""
CMP_FLAGS_GROUP_NAME = /D"_DOWNHILL_GROUP_NAME=\"$(DWN_GROUP_NAME)\""
!ELSE
CMP_FLAGS_GROUP_NAME =
!ENDIF
!IF "$(DWN_GROUP_IDREAL)" != ""
CMP_FLAGS_GROUP_IDREAL = /D"_DOWNHILL_GROUP_IDREAL=$(DWN_GROUP_IDREAL)"
!ELSE
CMP_FLAGS_GROUP_IDREAL =
!ENDIF
!IF "$(DWN_GROUP_IDEFFECTIVE)" != ""
CMP_FLAGS_GROUP_IDEFFECTIVE = /D"_DOWNHILL_GROUP_IDEFFECTIVE= \
			       $(DWN_GROUP_IDEFFECTIVE)"
!ELSE
CMP_FLAGS_GROUP_IDEFFECTIVE =
!ENDIF
!IF "$(DWN_GROUP_IDSAVED)" != ""
CMP_FLAGS_GROUP_IDSAVED = /D"_DOWNHILL_GROUP_IDSAVED=$(DWN_GROUP_IDSAVED)"
!ELSE
CMP_FLAGS_GROUP_IDSAVED =
!ENDIF
!IF "$(DWN_GROUP_PASSWORD)" != ""
CMP_FLAGS_GROUP_PASSWORD = /D"_DOWNHILL_GROUP_PASSWORD= \
			    \"$(DWN_GROUP_PASSWORD)\""
!ELSE
CMP_FLAGS_GROUP_PASSWORD =
!ENDIF
CMP_FLAGS          = /c /W3 /D"WIN32" $(CMP_FLAGS_PATH) $(CMP_FLAGS_DEBUG) \
		      $(CMP_FLAGS_UNIX)                                    \
		      $(CMP_FLAGS_MACHINE_TYPE)                            \
		      $(CMP_FLAGS_GETOPT_SWITCH)                           \
		      $(CMP_FLAGS_STAT_BLOCKSIZE)                          \
		      $(CMP_FLAGS_HOSTSEQUIV_FILE)                         \
		      $(CMP_FLAGS_RHOSTS_FILE)                             \
		      $(CMP_FLAGS_NETRC_FILE)                              \
		      $(CMP_FLAGS_USER_NAME)                               \
		      $(CMP_FLAGS_USER_IDREAL)                             \
		      $(CMP_FLAGS_USER_IDEFFECTIVE)                        \
		      $(CMP_FLAGS_USER_IDSAVED)                            \
		      $(CMP_FLAGS_USER_PASSWORD)                           \
		      $(CMP_FLAGS_USER_GECOS)                              \
		      $(CMP_FLAGS_USER_DIR)                                \
		      $(CMP_FLAGS_USER_SHELL)                              \
		      $(CMP_FLAGS_GROUP_NAME)                              \
		      $(CMP_FLAGS_GROUP_IDREAL)                            \
		      $(CMP_FLAGS_GROUP_IDEFFECTIVE)                       \
		      $(CMP_FLAGS_GROUP_IDSAVED)                           \
		      $(CMP_FLAGS_GROUP_PASSWORD)
#CMP_EXEC           = CL /MD /Fo
CMP_EXEC           = CL  /MD /Fo

# Archiver stuff ==============================================================
ARC_EXEC           = LIB /OUT:

# Rules =======================================================================
ALL: $(LIB_TARGET)

$(LIB_TARGET): $(INC_SYSTEM_DIR).H $(INC_POSIX_DEFINE) $(LIB_OBJS)
		@ ECHO ~~~~~ Archiving $(LIB_TARGET)
		@ $(ARC_EXEC)$(LIB_TARGET) $(LIB_OBJS)
		@ ECHO ~~~~~ $(LIB_TARGET) done

$(LIB_OBJS): $(LIB_SRCS) MAKEFILE $(INC_POSIX_DEFINE)
		@ ECHO ===== Compiling $(*).C
		@ $(CMP_EXEC)$(@) $(CMP_FLAGS) $(@D)\$(*F).C

$(INC_SYSTEM_DIR).H: MAKEFILE
		@ ECHO ~~~~~ Updating $(INC_SYSTEM_DIR).H
		@ COPY $(INC_SYSTEM_DIR).XXX $(INC_SYSTEM_DIR).H
		@ ECHO $(CMP_SYSTEM_DIR) >> $(INC_SYSTEM_DIR).H

$(INC_POSIX_DEFINE): MAKEFILE
		@ ECHO ~~~~~ Updating $(INC_POSIX_DEFINE)
!IF "$(CMP_POSIX)" == "YES"
		@ ECHO #define _DOWNHILL_POSIX_SOURCE 1 > $(INC_POSIX_DEFINE)
!ELSE
		@ ECHO #define _DOWNHILL_POSIX_SOURCE 0 > $(INC_POSIX_DEFINE)
!ENDIF

CLEAN:
		@ ECHO ----- Cleaning up
		@ DEL $(LIB_TARGET) $(LIB_OBJS) $(EXE_TARGET) $(EXE_OBJS) \
		  $(INC_SYSTEM_DIR).H $(INC_POSIX_DEFINE)
