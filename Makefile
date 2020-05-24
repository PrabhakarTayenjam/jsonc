CC = gcc -Wall -g
CC_RELEASE = gcc -Wall -fPIC
SRC = src
BIN = bin
TEST = test
RELEASE = release

.PHONY: release

default: clean build release

build: util strutil jsondatatype jsonparser jsonvalidator jsonerror

util: $(SRC)/util.c
	$(CC) -c $? -o $(BIN)/util.o
	$(CC_RELEASE) -c $? -o $(BIN)/util-release.o

strutil: $(SRC)/strutil.c
	$(CC) -c $? -o $(BIN)/strutil.o
	$(CC_RELEASE) -c $? -o $(BIN)/strutil-release.o

jsonerror: $(SRC)/jsonerror.c
	$(CC) -c $? -o $(BIN)/jsonerror.o
	$(CC_RELEASE) -c $? -o $(BIN)/jsonerror-release.o

jsondatatype: $(SRC)/jsondatatype.c
	$(CC) -c $? -o $(BIN)/jsondatatype.o
	$(CC_RELEASE) -c $? -o $(BIN)/jsondatatype-release.o

jsonparser: $(SRC)/jsonparser.c
	$(CC) -c $? -o $(BIN)/jsonparser.o
	$(CC_RELEASE) -c $? -o $(BIN)/jsonparser-release.o

jsonvalidator: $(SRC)/jsonvalidator.c
	$(CC) -c $? -o $(BIN)/jsonvalidator.o
	$(CC_RELEASE) -c $? -o $(BIN)/jsonvalidator-release.o

release:
	mkdir -p $(RELEASE)
	ar rcs $(RELEASE)/jsonc-debug.a $(BIN)/util.o $(BIN)/strutil.o $(BIN)/jsondatatype.o \
		$(BIN)/jsonparser.o $(BIN)/jsonvalidator.o $(BIN)/jsonerror.o

	ar rcs $(RELEASE)/jsonc.a $(BIN)/util-release.o $(BIN)/strutil-release.o \
		$(BIN)/jsondatatype-release.o $(BIN)/jsonparser-release.o $(BIN)/jsonvalidator-release.o \
		$(BIN)/jsonerror.o

	gcc -shared -o $(RELEASE)/jsonc.so $(BIN)/util-release.o $(BIN)/strutil-release.o \
		$(BIN)/jsondatatype-release.o $(BIN)/jsonparser-release.o $(BIN)/jsonvalidator-release.o \
		$(BIN)/jsonerror-release.o

clean:
	rm -rf $(BIN)/*