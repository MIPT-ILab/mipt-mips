.PHONY: install_libelf uninstall_libelf
.PHONY: install_mips-binutils uninstall_mips-binutils

.PHONY: install uninstall
.PHONY: clean


#########################################################################
#                             MIPS_BINUTILS
#########################################################################

# Variables for convinience:
PREFIX=/usr/local/mips-binutils
ALIAS_DIR=/usr/local/bin

AS= $(PREFIX)/bin/mipsel-unknown-linux-gnu-as
LD= $(PREFIX)/bin/mipsel-unknown-linux-gnu-ld
OB= $(PREFIX)/bin/mipsel-unknown-linux-gnu-objdump
MIPS_UTILS= $(AS) $(LD) $(OB)

ALIAS_AS= $(ALIAS_DIR)/mips-as
ALIAS_LD= $(ALIAS_DIR)/mips-ld
ALIAS_OB= $(ALIAS_DIR)/mips-objdump
ALIASES= $(ALIAS_AS) $(ALIAS_LD) $(ALIAS_OBJ)

DIR= binutils
ARCHIVE= binutils-2.28.tar.bz2
URL= http://ftp.gnu.org/gnu/$(DIR)/$(ARCHIVE)

install_mips-binutils: $(ALIASES)

# Create symlinks
$(ALIAS_AS): $(AS)
	ln -s $< $@
               
$(ALIAS_LD): $(LD)
	ln -s $< $@

$(ALIAS_OB): $(OB)
	ln -s $< $@


# Build and install binutils
$(MIPS_UTILS): $(DIR)/configure
	sudo mkdir -p $(PREFIX)
	@echo CLeaning previous builds...
	rm -rf build
	@mkdir build
	@echo Building library and installing...
	@cd build && ../$< --target=mipsel-unknown-linux-gnu --prefix=$(PREFIX) && \
	$(MAKE) && sudo $(MAKE) install

# Unpack archive
$(DIR)/configure: $(ARCHIVE)
	rm -rf $(DIR)
	@echo Extracting archive...
	@tar xjfm $<
	@mv binutils-2.28 binutils

# Download tarball
$(ARCHIVE):
	echo Downloading the source...
	@wget $(URL)


# Cleaning
clean:
	rm -rf $(DIR) $(ARCHIVE)

uninstall_mips-binutils:
	sudo rm -rf $(ALIASES) $(PREFIX)


#########################################################################
#                               LIBELF
#########################################################################

# This is just a possible way to install libelf on most popular systems

UNAME := $(shell uname -s)

ifeq ($(UNAME),Linux)
ELF_PREFIX= /usr/include
endif
ifeq ($(UNAME),Darwin)
ELF_PREFIX= /usr/local/include
endif

ELF_HEADERS= $(ELF_PREFIX)/libelf.h $(ELF_PREFIX)/gelf.h

install_libelf: $(ELF_HEADERS)

$(ELF_HEADERS): 
ifeq ($(UNAME),Linux)
	@echo This requires apt-get installed (Debian-based systems only) 
	sudo apt-get install libelf-dev
endif
ifeq ($(UNAME),Darwin)
	@echo Installing libelf on MacOS...
	brew install libelf
	@echo Creating symlinks for headers...
	@cd $(ELF_PREFIX) && ln -s libelf/libelf.h libelf.h && ln -s libelf/gelf.h gelf.h
endif


uninstall_libelf:
ifeq ($(UNAME),Linux)
	sudo apt-get purge libelf-dev
endif
ifeq ($(UNAME),Darwin)
	@cd $(ELF_PREFIX) && sudo rm libelf.h gelf.h
	brew uninstall libelf
endif


install: install_mips-binutils install_libelf
uninstall: uninstall_mips-binutils uninstall_libelf
