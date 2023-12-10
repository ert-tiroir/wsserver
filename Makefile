
SOURCEDIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/build
SOURCES   := $(shell find $(SOURCEDIR) -name '*.cpp')

clean:
	-[ -e build/out ] && rm -f build/out
	-[ -e out ] && rm -f out
	-[ -e build/rpi ] && rm -rf build/rpi
build:
	mkdir -p build
	make -B clean

	cp -r wsserver build
	cp entry.cpp build/

	make -B compile
compile:
	g++ -o build/out -I./build $(SOURCES)
	cp build/out .
run:
	make -B build
	./out
