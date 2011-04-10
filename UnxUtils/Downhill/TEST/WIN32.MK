# *****************************************************************************
# *                                                                           *
# * WIN32.MK                                                                  *
# *                                                                           *
# * Freely redistributable and modifiable.  Use at your own risk.             *
# *                                                                           *
# * Copyright 1994 The Downhill Project                                       *
# *                                                                           *
# *****************************************************************************


# User controlled settings ****************************************************

# How things are compiled =====================================================
CMP_DEBUG          = NO                    # Turn on debugging for compiles?
LNK_DEBUG          = NO                    # Turn on debugging for linking?


# Things the user should keep his mits off ************************************

# Targets and objects =========================================================
EXE_TARGET         = DOWNHILL.EXE
EXE_OBJS           = DOWNHILL.OBJ

# Compiler stuff ==============================================================
!IF "$(CMP_DEBUG)" == "YES"
CMP_FLAGS_DEBUG    = -Zi -Od
!ELSE
CMP_FLAGS_DEBUG    = -Ox
!ENDIF
CMP_FLAGS_PATH     = -I"..\INCLUDE"
CMP_FLAGS          = -c -W3 $(CMP_FLAGS_PATH) $(CMP_FLAGS_DEBUG) -DWIN32
CMP_EXEC           = CL /Fo

# Linker stuff ================================================================
LNK_EXEC           = LINK /out:
!IF "$(LNK_DEBUG)" == "YES"
LNK_FLAGS_DEBUG    = -debug:full -debugtype:cv
!ELSE
LNK_FLAGS_DEBUG    =
!ENDIF
LNK_LIB_DOWNHILL   = ..\DOWNHILL.LIB
LNK_LIBS           = LIBC.LIB         \
                      KERNEL32.LIB    \
                      WSOCK32.LIB     \
                      $(LNK_LIB_DOWNHILL)
LNK_FLAGS          = -subsystem:console -entry:mainCRTStartup -align:0x1000 \
                      $(LNK_FLAGS_DEBUG)

# Rules =======================================================================
ALL: $(EXE_TARGET)

$(EXE_TARGET): $(LNK_LIB_DOWNHILL) $(EXE_OBJS)
		@ ECHO ~~~~~ Linking $(EXE_TARGET)
		@ $(LNK_EXEC)$(@) $(LNK_FLAGS) $(EXE_OBJS) $(LNK_LIBS)
		@ ECHO ~~~~~ $(EXE_TARGET) done

.C.OBJ:
		@ ECHO ===== Compiling $(*).C
		@ $(CMP_EXEC)$(@) $(CMP_FLAGS) $(@D)\$(*F).C

$(LNK_LIB_DOWNHILL):
		@ CD ..
		@ NMAKE
		@ CD TEST

CLEAN:
		@ ECHO ----- Cleaning up
		@ DEL $(EXE_TARGET) $(EXE_OBJS)
