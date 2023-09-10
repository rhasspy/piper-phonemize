.PHONY: clean

all:
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=install
	cmake --build build --config Release
	cd build && ctest --config Release
	cmake --install install

clean:
	rm -rf build install
