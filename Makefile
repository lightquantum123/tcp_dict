UNIF = unittest

UNIE = db_test dic_test transcode_test

export UNIE

server: db.o dic.o transcode.o dicserver.o
	gcc $^ -o $@ -lsqlite3

db.o: db.c
	gcc -c $^ -o $@ -lsqlite3

dic.o: dic.c
	gcc -c $^ -o $@

transcode.o: transcode.c
	gcc -c $^ -o $@

dicserver.o: dicserver.c
	gcc -c $^ -o $@

unitt: db_test dic_test transcode_test

db_test: unittest/db_test.c db.o
	$(MAKE) -C $(UNIF) $@

dic_test: unittest/dic_test.c dic.o
	$(MAKE) -C $(UNIF) $@

transcode_test: unittest/trancode_test.c transcode.o
	$(MAKE) -C $(UNIF) $@

.PHONY clean:
	$(MAKE) -C $(UNIF) $@; \
	rm -f *.o server client

