DIRS = src app
TARGETS = all clean install

$(TARGETS): %: $(patsubst %, %.%, $(DIRS))

$(foreach TGT, $(TARGETS), $(patsubst %, %.$(TGT), $(DIRS))):
	$(MAKE) -C $(subst ., , $@)
	
docker:
	docker build -t udt/clisrv -f Dockerfile-UDT .
