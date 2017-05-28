STDTAR := all check install uninstall clean

.phony: $(STDTAR)
$(STDTAR):
	make TARGET="$@" run_command
	
.phony: run_command
run_command:
	$(MAKE) -C game_server $(TARGET)
