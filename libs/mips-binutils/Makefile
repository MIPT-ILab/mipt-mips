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
	rm -rf build
	mkdir build
	cd build && ../$< --target=mipsel-unknown-linux-gnu --prefix=$(PREFIX) && \
	$(MAKE) && $(MAKE) install

# Unpack archive
$(DIR)/configure: $(ARCHIVE)
	rm -rf $(DIR)
	tar xjfm $<
	mv binutils-2.28 binutils

# Download tarball
$(ARCHIVE):
	wget $(URL)


# Cleaning
clean:
	rm -rf $(DIR) $(ARCHIVE)

uninstall_mips-binutils:
	rm -rf $(PREFIX)
