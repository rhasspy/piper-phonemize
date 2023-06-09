.PHONY: release test python python-test docker

LIB_DIR := lib/Linux-$(shell uname -m)

release:
	mkdir -p build
	cd build && PKG_CONFIG_PATH='../espeak-ng/build/lib/pkgconfig' cmake .. -DCMAKE_BUILD_TYPE=Release && make
	cp -a espeak-ng/build/lib/libespeak*.so* build/
	cp -R espeak-ng/build/share/espeak-ng-data build/
	cp -a $(LIB_DIR)/onnxruntime/lib/libonnxruntime*.so* build/

test:
	g++ -Wall -o test -Iespeak-ng/build/include -Lespeak-ng/build/lib -I$(LIB_DIR)/onnxruntime/include -L$(LIB_DIR)/onnxruntime/lib -Isrc -std=c++17 src/test.cpp src/phonemize.cpp src/phoneme_ids.cpp src/tashkeel.cpp -lespeak-ng -lonnxruntime
	LD_LIBRARY_PATH="espeak-ng/build/lib:$(LIB_DIR)/onnxruntime/lib" ./test 'espeak-ng/build/share/espeak-ng-data'

python:
	LD_LIBRARY_PATH='espeak-ng/build/lib' .venv/bin/pip3 install -e .

python-test:
	LD_LIBRARY_PATH='espeak-ng/build/lib' python3 src/python_test.py

docker:
	docker buildx build . --platform 'linux/amd64,linux/arm64' --output 'type=local,dest=dist'
