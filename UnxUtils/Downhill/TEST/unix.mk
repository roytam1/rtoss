# *****************************************************************************
# *                                                                           *
# * unix.mk                                                                   *
# *                                                                           *
# * Freely redistributable and modifiable.  Use at your own risk.             *
# *                                                                           *
# * Copyright 1994 The Downhill Project                                       *
# *                                                                           *
# *****************************************************************************


# User controlled settings ****************************************************

# How things are compiled =====================================================
CMP_FLAGS_DEBUG    =                                   # Debugging flags for cc


# Things the user should keep his mits off ************************************

# Targets and objects =========================================================
EXE_TARGET         = DOWNHILL
EXE_OBJS           = DOWNHILL.o

# Compiler stuff ==============================================================
CMP_FLAGS          = -c $(CMP_FLAGS_PATH) $(CMP_FLAGS_DEBUG)
CMP_EXEC           = cc

# Linker stuff ================================================================
LNK_EXEC           = cc -o

# Rules =======================================================================
all: $(EXE_TARGET)

$(EXE_TARGET): $(EXE_OBJS)
		@ echo ~~~~~ Linking $(EXE_TARGET)
		@ $(LNK_EXEC) $(@) $(EXE_OBJS)
		@ echo ~~~~~ $(EXE_TARGET) done

.C.o:
		@ echo ===== Compiling $(*).C
		@ cp $(*).C $(*).c
		@ $(CMP_EXEC) $(CMP_FLAGS) $(*).c
		@ rm -f $(*).c

clean:
		@ echo ----- Cleaning up
		@ rm -f $(EXE_TARGET) $(EXE_OBJS)
