## -*- Makefile -*-
##
## User: murilo
## Time: Sep 28, 2021 10:13:03 PM
## Makefile created by Oracle Developer Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CC = gcc
BASICOPTS = -g -Wall
CFLAGS = $(BASICOPTS)


# Define the target directories.
TARGETDIR_build=output


all: $(TARGETDIR_build)/build

## Target: build
CPPFLAGS_build = \
	-I../../src
OBJS_build =  \
	$(TARGETDIR_build)/main.o \
	$(TARGETDIR_build)/libconfigfile.o


# Link or archive
$(TARGETDIR_build)/build: $(TARGETDIR_build) $(OBJS_build) $(DEPLIBS_build)
	$(LINK.c) $(CFLAGS_build) $(CPPFLAGS_build) -o $@ $(OBJS_build) $(LDLIBS_build)


# Compile source files into .o files
$(TARGETDIR_build)/main.o: $(TARGETDIR_build) main.c
	$(COMPILE.c) $(CFLAGS_build) $(CPPFLAGS_build) -o $@ main.c

$(TARGETDIR_build)/libconfigfile.o: $(TARGETDIR_build) ../../src/libconfigfile.c
	$(COMPILE.c) $(CFLAGS_build) $(CPPFLAGS_build) -o $@ ../../src/libconfigfile.c



#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_build)/build \
		$(TARGETDIR_build)/main.o \
		$(TARGETDIR_build)/libconfigfile.o
	rm -f -r $(TARGETDIR_build)


# Create the target directory (if needed)
$(TARGETDIR_build):
	mkdir -p $(TARGETDIR_build)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

