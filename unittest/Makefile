PARDIR = ..

db_test: $(PARDIR)/db.o db_test.c
	gcc $^ -o $@ -lsqlite3

dic_test: $(PARDIR)/dic.o dic_test.c
	gcc $^ -o $@

transcode_test: $(PARDIR)/transcode.o trancode_test.c
	gcc $^ -o $@

.PHONY clean:
	rm -f UNIE
