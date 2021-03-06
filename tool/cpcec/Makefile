SHELL=/bin/bash

VARIABLES_AT_MAKEFILE_START := $(.VARIABLES)

TARGETS=build_config.inc

.PHONY: all

all: $(TARGETS)

########################################################################
# Variables to adjust at each new integrated project
########################################################################

PRODUCT_NAME=cpcec
# URL_GIT=https://github.com/cpcitor/cpcec # unused, set in git submodule

# You must set EXTRACT_DIR_NAME to match archive content.
EXTRACT_DIR_NAME=$(PRODUCT_NAME)

# If only one generated executable file, set BUILD_TARGET_FILE to path to it.
# It several generated executable files, do not set BUILD_TARGET_FILE.
BUILD_DIR=$(EXTRACT_DIR_NAME)
BUILD_TARGET_FILE=$(BUILD_DIR)/${PRODUCT_NAME}

$(EXTRACT_DIR_NAME)/.unpacked $(EXTRACT_DIR_NAME)/Makefile: Makefile.cpcec
	@echo "************************************************************************"
	@echo "**************** Extracting source from: $^"
	@echo "************************************************************************"
	git submodule update --init $(PRODUCT_NAME)
	cp Makefile.cpcec $(EXTRACT_DIR_NAME)/Makefile
	touch $@
	@echo "************************************************************************"
	@echo "**************** Source extracted to: $(@D)"
	@echo "************************************************************************"

PATCHNAME=$(EXTRACT_DIR_NAME)_build_patch.patch

.PHONY: patch

patch: $(EXTRACT_DIR_NAME)/.patched

ifeq ($(wildcard $(PATCHNAME)),)
$(EXTRACT_DIR_NAME)/.patched: $(EXTRACT_DIR_NAME)/.unpacked
	@echo "************************************************************************"
	@echo "**************** No patch to apply to: $^"
	@echo "************************************************************************"
	touch $@
	@echo "************************************************************************"
	@echo "**************** Source patched in: $(@D)"
	@echo "************************************************************************"
else
$(EXTRACT_DIR_NAME)/.patched: $(PATCHNAME) $(EXTRACT_DIR_NAME)/.unpacked
	@echo "************************************************************************"
	@echo "**************** Patching source in: $^"
	@echo "************************************************************************"
	patch -p0 < $<
	touch $@
	@echo "************************************************************************"
	@echo "**************** Source patched in: $(@D)"
	@echo "************************************************************************"
endif

BUILD_TARGET_FILE?=$(EXTRACT_DIR_NAME)/.built

$(BUILD_TARGET_FILE): $(EXTRACT_DIR_NAME)/.patched $(EXTRACT_DIR_NAME)/Makefile
	@echo "************************************************************************"
	@echo "**************** Configuring and build in: $^"
	@echo "************************************************************************"
	$(MAKE) -C "$(BUILD_DIR)" --print-directory
	touch "$@"
	@echo "************************************************************************"
	@echo "**************** Configured and built in: $(@D)"
	@echo "************************************************************************"

build_config.inc: bin/cpcec cpcec/cpcec Makefile
	(set -eu ; \
	{ \
	echo "# with bash do \"source\" this file." ; \
	cd "$(<D)" ; \
	echo "export PATH=\"\$${PATH}:$$PWD\"" ; \
	} >$@ ; )

run: build_config.inc
	( . build_config.inc ; $(PRODUCT_NAME) )

clean:
	-$(MAKE) -C "$(BUILD_DIR)" clean
	-rm -f $(BUILD_TARGET_FILE)

mrproper:
	-rm -f $(BUILD_TARGET_FILE)
	-rm -f $(TARGETS)
	-rm -rf $(EXTRACT_DIR_NAME) ._$(EXTRACT_DIR_NAME) *~
	-rm -f $(PRODUCT_NAME)

distclean: mrproper


########################################################################
# Debug the makefile
########################################################################
$(foreach v,                                        \
  $(filter-out $(VARIABLES_AT_MAKEFILE_START) VARIABLES_AT_MAKEFILE_START,$(.VARIABLES)), \
  $(info $(v) = $($(v))))
