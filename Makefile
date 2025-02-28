ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif

include $(DEVKITPRO)/devkitA64/base_tools

TOPTARGETS := all clean dist-no-debug dist
AMSBRANCH := $(shell git symbolic-ref --short HEAD)
AMSHASH := $(shell git rev-parse --short HEAD)
AMSREV := $(AMSBRANCH)-$(AMSHASH)

ifneq (, $(strip $(shell git status --porcelain 2>/dev/null)))
    AMSREV := $(AMSREV)-dirty
endif

COMPONENTS := fusee stratosphere mesosphere exosphere emummc thermosphere troposphere libraries

all: $(COMPONENTS)
	$(eval MAJORVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MAJOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval MINORVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MINOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval MICROVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MICRO\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval S_MAJORVER = $(shell grep 'define ATMOSPHERE_SUPPORTED_HOS_VERSION_MAJOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval S_MINORVER = $(shell grep 'define ATMOSPHERE_SUPPORTED_HOS_VERSION_MINOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval S_MICROVER = $(shell grep 'define ATMOSPHERE_SUPPORTED_HOS_VERSION_MICRO\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	@python fusee/build_package3.py $(CURDIR) release $(AMSHASH) $(MAJORVER) $(MINORVER) $(MICROVER) 0 $(S_MAJORVER) $(S_MINORVER) $(S_MICROVER) 0
	@echo "Built package3!"

thermosphere:
	$(MAKE) -C thermosphere all

exosphere: thermosphere
	$(MAKE) -C exosphere all

stratosphere: exosphere libraries
	$(MAKE) -C stratosphere all

mesosphere: exosphere libraries
	$(MAKE) -C mesosphere all

troposphere: stratosphere
	$(MAKE) -C troposphere all

emummc:
	$(MAKE) -C emummc all

fusee: exosphere mesosphere stratosphere
	$(MAKE) -C $@ all

libraries:
	$(MAKE) -C libraries all

clean:
	$(MAKE) -C fusee clean
	$(MAKE) -C emummc clean
	$(MAKE) -C libraries clean
	$(MAKE) -C exosphere clean
	$(MAKE) -C thermosphere clean
	$(MAKE) -C mesosphere clean
	$(MAKE) -C stratosphere clean
	rm -rf out

dist-no-debug: all
	$(eval MAJORVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MAJOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval MINORVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MINOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval MICROVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MICRO\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval AMSVER = $(MAJORVER).$(MINORVER).$(MICROVER)-$(AMSREV))
	rm -rf atmosphere-$(AMSVER)
	rm -rf out
	mkdir atmosphere-$(AMSVER)
	mkdir atmosphere-$(AMSVER)/atmosphere
	mkdir atmosphere-$(AMSVER)/switch
	mkdir -p atmosphere-$(AMSVER)/atmosphere/fatal_errors
	mkdir -p atmosphere-$(AMSVER)/atmosphere/config_templates
	mkdir -p atmosphere-$(AMSVER)/atmosphere/config
	mkdir -p atmosphere-$(AMSVER)/atmosphere/flags
	cp fusee/fusee.bin atmosphere-$(AMSVER)/atmosphere/reboot_payload.bin
	cp fusee/package3 atmosphere-$(AMSVER)/atmosphere/package3
	cp config_templates/stratosphere.ini atmosphere-$(AMSVER)/atmosphere/config_templates/stratosphere.ini
	cp config_templates/override_config.ini atmosphere-$(AMSVER)/atmosphere/config_templates/override_config.ini
	cp config_templates/system_settings.ini atmosphere-$(AMSVER)/atmosphere/config_templates/system_settings.ini
	cp config_templates/exosphere.ini atmosphere-$(AMSVER)/atmosphere/config_templates/exosphere.ini
	mkdir -p config_templates/kip_patches
	cp -r config_templates/kip_patches atmosphere-$(AMSVER)/atmosphere/kip_patches
	cp -r config_templates/hbl_html atmosphere-$(AMSVER)/atmosphere/hbl_html
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000008
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000000D
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000017
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000002B
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000032
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000034
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000036
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000037
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000003C
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000042
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000420
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000B240
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000D609
	mkdir -p atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000D623
	cp stratosphere/boot2/boot2.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000008/exefs.nsp
	cp stratosphere/dmnt/dmnt.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000000D/exefs.nsp
	cp stratosphere/cs/cs.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000017/exefs.nsp
	cp stratosphere/erpt/erpt.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000002B/exefs.nsp
	cp stratosphere/eclct.stub/eclct.stub.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000032/exefs.nsp
	cp stratosphere/fatal/fatal.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000034/exefs.nsp
	cp stratosphere/creport/creport.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000036/exefs.nsp
	cp stratosphere/ro/ro.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000037/exefs.nsp
	cp stratosphere/jpegdec/jpegdec.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000003C/exefs.nsp
	cp stratosphere/pgl/pgl.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000042/exefs.nsp
	cp stratosphere/LogManager/LogManager.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/0100000000000420/exefs.nsp
	cp stratosphere/htc/htc.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000B240/exefs.nsp
	cp stratosphere/dmnt.gen2/dmnt.gen2.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000D609/exefs.nsp
	cp stratosphere/TioServer/TioServer.nsp atmosphere-$(AMSVER)/stratosphere_romfs/atmosphere/contents/010000000000D623/exefs.nsp
	@build_romfs atmosphere-$(AMSVER)/stratosphere_romfs atmosphere-$(AMSVER)/atmosphere/stratosphere.romfs
	rm -r atmosphere-$(AMSVER)/stratosphere_romfs
	cp troposphere/reboot_to_payload/reboot_to_payload.nro atmosphere-$(AMSVER)/switch/reboot_to_payload.nro
	cp troposphere/daybreak/daybreak.nro atmosphere-$(AMSVER)/switch/daybreak.nro
	cd atmosphere-$(AMSVER); zip -r ../atmosphere-$(AMSVER).zip ./*; cd ../;
	rm -rf atmosphere-$(AMSVER)
	mkdir out
	mv atmosphere-$(AMSVER).zip out/atmosphere-$(AMSVER).zip
	cp fusee/fusee.bin out/fusee.bin

dist: dist-no-debug
	$(eval MAJORVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MAJOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval MINORVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MINOR\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval MICROVER = $(shell grep 'define ATMOSPHERE_RELEASE_VERSION_MICRO\b' libraries/libvapours/include/vapours/ams/ams_api_version.h \
		| tr -s [:blank:] \
		| cut -d' ' -f3))
	$(eval AMSVER = $(MAJORVER).$(MINORVER).$(MICROVER)-$(AMSREV))
	rm -rf atmosphere-$(AMSVER)-debug
	mkdir atmosphere-$(AMSVER)-debug
	cp fusee/loader_stub/loader_stub.elf atmosphere-$(AMSVER)-debug/fusee-loader-stub.elf
	cp fusee/program/program.elf atmosphere-$(AMSVER)-debug/fusee-program.elf
	cp exosphere/loader_stub/loader_stub.elf atmosphere-$(AMSVER)-debug/exosphere-loader-stub.elf
	cp exosphere/program/program.elf atmosphere-$(AMSVER)-debug/exosphere-program.elf
	cp exosphere/warmboot/warmboot.elf atmosphere-$(AMSVER)-debug/exosphere-warmboot.elf
	cp exosphere/mariko_fatal/mariko_fatal.elf atmosphere-$(AMSVER)-debug/exosphere-mariko-fatal.elf
	cp exosphere/program/sc7fw/sc7fw.elf atmosphere-$(AMSVER)-debug/exosphere-sc7fw.elf
	cp exosphere/program/rebootstub/rebootstub.elf atmosphere-$(AMSVER)-debug/exosphere-rebootstub.elf
	cp mesosphere/kernel_ldr/kernel_ldr.elf atmosphere-$(AMSVER)-debug/kernel_ldr.elf
	cp mesosphere/kernel/kernel.elf atmosphere-$(AMSVER)-debug/kernel.elf
	cp stratosphere/ams_mitm/ams_mitm.elf atmosphere-$(AMSVER)-debug/ams_mitm.elf
	cp stratosphere/boot/boot.elf atmosphere-$(AMSVER)-debug/boot.elf
	cp stratosphere/boot2/boot2.elf atmosphere-$(AMSVER)-debug/boot2.elf
	cp stratosphere/creport/creport.elf atmosphere-$(AMSVER)-debug/creport.elf
	cp stratosphere/dmnt/dmnt.elf atmosphere-$(AMSVER)-debug/dmnt.elf
	cp stratosphere/eclct.stub/eclct.stub.elf atmosphere-$(AMSVER)-debug/eclct.stub.elf
	cp stratosphere/erpt/erpt.elf atmosphere-$(AMSVER)-debug/erpt.elf
	cp stratosphere/fatal/fatal.elf atmosphere-$(AMSVER)-debug/fatal.elf
	cp stratosphere/jpegdec/jpegdec.elf atmosphere-$(AMSVER)-debug/jpegdec.elf
	cp stratosphere/loader/loader.elf atmosphere-$(AMSVER)-debug/loader.elf
	cp stratosphere/ncm/ncm.elf atmosphere-$(AMSVER)-debug/ncm.elf
	cp stratosphere/pgl/pgl.elf atmosphere-$(AMSVER)-debug/pgl.elf
	cp stratosphere/pm/pm.elf atmosphere-$(AMSVER)-debug/pm.elf
	cp stratosphere/ro/ro.elf atmosphere-$(AMSVER)-debug/ro.elf
	cp stratosphere/sm/sm.elf atmosphere-$(AMSVER)-debug/sm.elf
	cp stratosphere/spl/spl.elf atmosphere-$(AMSVER)-debug/spl.elf
	cp troposphere/daybreak/daybreak.elf atmosphere-$(AMSVER)-debug/daybreak.elf
	cd atmosphere-$(AMSVER)-debug; zip -r ../atmosphere-$(AMSVER)-debug.zip ./*; cd ../;
	rm -r atmosphere-$(AMSVER)-debug
	mv atmosphere-$(AMSVER)-debug.zip out/atmosphere-$(AMSVER)-debug.zip


.PHONY: $(TOPTARGETS) $(COMPONENTS)
