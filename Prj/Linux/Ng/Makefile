MPDIR = ../../../LqbsSoft

SUBDIRS	= $(MPDIR)/App/Cct \
			$(MPDIR)/App/DB \
			$(MPDIR)/App/DB/SQLite \
			$(MPDIR)/App/Proto \
			$(MPDIR)/App/Proto/If \
			$(MPDIR)/App/Proto/CctAppAdapter \
			$(MPDIR)/Prj/Linux/sysarch \
			$(MPDIR)/Prj/Linux/main
all:

	$(foreach dir, $(SUBDIRS), $(MAKE) -C $(dir) ;)

clean:
	$(foreach dir, $(SUBDIRS), $(MAKE) -C $(dir) clean ;)
	rm -f lib/*.a

cleanup:
	@(rm -r -f bin lib obj)