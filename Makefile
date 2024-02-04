.PHONY: clean

all:
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=install
	cmake --build build --config Release
	@if [ "$(OS)" != "Windows_NT" ]; then \
        cd build && ctest --config Release; \
    fi
	cmake --install build

clean:
	rm -rf build install
