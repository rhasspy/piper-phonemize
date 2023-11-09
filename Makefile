.PHONY: clean

all:
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$${PREFIX:-install}" -DBUILD_EXTERNAL="$${BUILD_EXTERNAL:-ON}"
	cmake --build build --config Release
	cd build && ctest --config Release
	cmake --install build

clean:
	rm -rf build install
