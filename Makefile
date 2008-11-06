TARGETS = dping dpong

DBUS_INCS=`pkg-config dbus-glib-1 --cflags` 
DBUS_LIBS=`pkg-config dbus-glib-1 --libs`

all: $(TARGETS)

clean:
	$(RM) *.o *~

distclean: clean
	$(RM) $(TARGETS)

dping: dping.c utils.c
	gcc -W -Wall -O2 -s $(DBUS_INCS) $(DBUS_LIBS) -lrt -o $@ $^ 

dpong: dpong.c utils.c
	gcc -W -Wall -O2 -s $(DBUS_INCS) $(DBUS_LIBS) -lrt -o $@ $^ 

install:

	mkdir -p $(DESTDIR)/usr/bin
	install dping $(DESTDIR)/usr/bin/
	install dpong $(DESTDIR)/usr/bin/
