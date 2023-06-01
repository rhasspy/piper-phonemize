.PHONY: test python

all:
	g++ -shared -fPIC -Wall -o libpiper-phonemize.so -Iespeak-ng/build/include -Lespeak-ng/build/lib -Isrc -std=c++17 src/phonemize.hpp src/phoneme_ids.hpp -lespeak-ng

test:
	g++ -Wall -o test -Iespeak-ng/build/include -Lespeak-ng/build/lib -Isrc -std=c++17 src/test.cpp -lespeak-ng
	LD_LIBRARY_PATH='espeak-ng/build/lib' ./test 'espeak-ng/build/share/espeak-ng-data'

python:
	LD_LIBRARY_PATH='espeak-ng/build/lib' .venv/bin/pip3 install -e .
