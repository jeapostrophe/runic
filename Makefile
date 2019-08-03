TOPTARGETS := all clean

SUBDIRS := runic test wordlist/v1 wordlist/v2
IGNORES :=  wordlist/v2/PicoSHA2
RUNS := $(filter-out $(IGNORES), $(SUBDIRS))

$(TOPTARGETS): $(RUNS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
