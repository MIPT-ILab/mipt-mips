.PHONY: install_libelf install_libelf_mac install_libelf_debian
.PHONY: uninstall_libelf uninstall_libelf_mac uninstall_libelf_debian

.PHONY: install_mips-binutils uninstall_mips-binutils

.PHONY: install uninstall
.PHONY: clean

install: install_mips-binutils install_libelf
uninstall: uninstall_mips-binutils uninstall_libelf

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
	rm -rf $(PREFIX)


#########################################################################
#                               LIBELF
#########################################################################

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
LIBELF_INSTALL= install_libelf_debian
LIBELF_UNINSTALL= uninstall_libelf_debian
endif
ifeq ($(UNAME),Darwin)
LIBELF_INSTALL= install_libelf_mac
LIBELF_UNINSTALL= uninstall_libelf_mac
endif

install_libelf: $(LIBELF_INSTALL)

install_libelf_debian:
	@echo This requires apt-get installed (Debian-based systems only) 
	sudo apt-get install libelf-dev

install_libelf_mac:
	@echo Installing libelf on MacOS...
	brew install libelf
	@echo Creating symlinks for headers...
	@cd /usr/local/include && ln -s libelf/libelf.h libelf.h && ln -s libelf/gelf.h gelf.h

uninstall_libelf: $(LIBELF_UNINSTALL)

uninstall_libelf_debian:
	sudo apt-get purge libelf-dev

uinstall_libelf_mac:
	rm /usr/local/include/libelf.h /usr/local/include/gelf.h
	brew uninstall libelf
