default:
	g++ -Wall -o main -Iespeak-ng/build/include -Lespeak-ng/build/lib -Isrc -std=c++17  src/main.cpp -lespeak-ng
