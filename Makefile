TOPTARGETS := all clean

SUBDIRS := runic test wordlist/v1 wordlist/v2

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
