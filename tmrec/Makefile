# $Id: Makefile,v 1.2 2004-03-31 09:58:31 dcm Exp $
#
# root make

include Makefile.defs

#************************* RULES *****************************


#common rules

.PHONY: all
all:
	make -C $(SRC) all

.PHONY: dep
dep:
	make -C $(SRC) dep

.PHONY: bin
bin:
	make -C $(SRC) bin

.PHONY: TAGS
TAGS:
	ctags -R .

.PHONY: dist
dist: proper
	tar --exclude .*.swp --exclude ~* --exclude ~*.* --exclude tags \
		--exclude *.d --exclude *.o --exclude *.so \
		-zcf ../$(NAME)-$(RELEASE).tar.gz *

.PHONY: snap
snap: proper
	tar --exclude .*.swp --exclude ~* --exclude ~*.* --exclude tags \
		--exclude *.d --exclude *.o --exclude *.so \
		-zcf ../$(NAME)-`date +%Y%m%d-%H%M`.tar.gz *

.PHONY: clean
clean:
	make -C $(SRC) clean

.PHONY: proper
proper:
	make -C $(SRC) proper

