# -------------------------------------------
# Makefile
#
# Written by Ed Zavada, 2012
# Copyright (c) 2012, Dream Rock Studios, LLC
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit
# persons to whom the Software is furnished to do so, subject to the
# following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
# NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
# USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# -------------------------------------------

TESTS = test/*.js
PLATFORM = $(shell uname)

all: build

build: clean configure compile

configure:
	node-gyp configure

compile: configure
	node-gyp build
ifeq ($(PLATFORM),Darwin)
	-@nm -mu build/Release/pdg.node | grep "dynamically looked" | grep -v "__ZNK2v8" \
		| grep -v "__ZN4node" | grep -v "__ZN2v8" | grep -v "_ev_" | grep -v "_MOZ_Z_"
else
	-@nm -u build/Release/pdg.node | grep -v "@@" | grep -v "_ZNK2v8" \
		| grep -v "_ZN4node" | grep -v "_ZN2v8" | grep -v " ev_" | grep -v "^[ \t]*w "
endif

compile-verbose: 
	node-gyp -verbose build

test: 
	echo "TODO: add tests to node package"

clean:
	rm -f pdg.node
	rm -Rf build
	rm -f .lock-wscript
	rm -f src/.lock-wscript


.PHONY: all clean test build configure compile compile-verbose

