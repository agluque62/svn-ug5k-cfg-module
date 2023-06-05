################################################################
.SILENT:
SHELL  := /bin/bash
OSNAME := $(shell uname -s)
################################################################
## Colordefinition
################################################################
NO_COLOR    = \x1b[0m
OK_COLOR    = \x1b[32;01m
WARN_COLOR  = \x1b[33;01m
ERROR_COLOR = \x1b[31;01m
################################################################

OBJ01 = obj/uv5ki-gw-cfg.o
OBJ02 = obj/code-base.o obj/exception.o obj/sistema.o obj/socket.o obj/thread.o obj/sqlite3.o obj/sqlite3++.o obj/md5.o
OBJ03 = obj/mongoose.o obj/web-app-server.o obj/uv5kigwcfg-web-app.o obj/uv5kigwcfg-web-app-data.o 
OBJ04 = obj/stlini.o obj/base64.o obj/tools.o obj/parse.o obj/ftp-client.o obj/http-client.o
OBJ05 = obj/local-config.o obj/working-config.o obj/comm-config.o obj/comm-conversor.o obj/comm-preconf.o obj/soap-config.o
OBJ06 = obj/cfg-proc.o obj/man-proc.o obj/his-proc.o obj/file-supervisor.o

OBJS  = $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) obj/buildstring.o
CROSS_COMPILE = ppc_82xx-
CC    = $(CROSS_COMPILE)gcc

ifeq ($(CROSS_COMPILE),ppc_82xx-)
	CFLAGS	= -Wall -Wno-unknown-pragmas -ggdb -I. -D_PPC82xx_ 
	STRIP = strip uvki-gwser
else
	CFLAGS	= -Wall -Wno-unknown-pragmas -ggdb -I.
	STRIP = 
endif
##CFLAGS	= -Wall -Wno-unknown-pragmas -ggdb -I. -I./mongoose -I./Parse -I./rapidjson
LIBS	=-lpthread -lstdc++ -ldl
ifeq ($(OSNAME),Linux)
    LDFLAGS=-Wl,-Map=uvki-gwser.map
endif
ifeq ($(OSNAME),Darwin)
    LDFLAGS=
endif

##LDFLAGS	=-Wl,-Map=ug5kser.map

all: uvki-gwser

obj/buildstring.o:
	date '+char acBuildString[] = "NUCLEODF %F %H:%M:%S";' > ./buildstring.cxx
##	date '+char acBuildString[] = "NUCLEODF 2022-09-21 17:30:33";' > ./buildstring.cxx
	$(CC) -c $(CFLAGS) ./buildstring.cxx -o ./obj/buildstring.o

obj/%.o: ./src/base/%.c
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@
	
obj/%.o: ./src/base/%.cpp
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: ./src/websrv/%.cpp
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: ./src/websrv/%.c
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: ./src/tools/%.cpp
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: ./src/config/%.cpp
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: ./src/%.cpp
	# @echo --------------------------------------------------------------------------------
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

obj/%.o: ./%.cpp
	# @echo --------------------------------------------------------------------------------
	echo  -e "---CROSS_COMPILE---$(CROSS_COMPILE) "
	echo  -e "$(WARN_COLOR)-- $(CROSS_COMPILE) Compiling $<$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) -c $(CFLAGS) $< -o $@

uvki-gwser: $(OBJS)
	#@echo --------------------------------------------------------------------------------
	echo  -e "$(OK_COLOR)--  OS=$(OSNAME)$(NO_COLOR)"
	echo  -e "$(OK_COLOR)--  Linking $@$(NO_COLOR)"
	# @echo --------------------------------------------------------------------------------
	$(CC) $(LIBS) $(LDFLAGS) -o $@ $(OBJS)
	#$(CROSS_COMPILE)strip uvki-gwser
	echo -e "$(OK_COLOR)--  STRIP $(CROSS_COMPILE)$(STRIP)$(NO_COLOR)"
	$(CROSS_COMPILE)$(STRIP)
	# @echo --------------------------------------------------------------------------------
	echo

clean:
	echo  -e "$(ERROR_COLOR)-- Eliminando Ficheros objeto...$(NO_COLOR)"
	rm -r obj/*.o
	echo

