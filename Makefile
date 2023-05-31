.PHONY: main test

main:
	g++ -Wall -o main -Iespeak-ng/build/include -Lespeak-ng/build/lib -Isrc -std=c++17 src/main.cpp -lespeak-ng

test:
	g++ -Wall -o test -Iespeak-ng/build/include -Lespeak-ng/build/lib -Isrc -std=c++17 src/test.cpp -lespeak-ng
	LD_LIBRARY_PATH='espeak-ng/build/lib' ./test 'espeak-ng/build/share/espeak-ng-data'
