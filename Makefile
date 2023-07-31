.PHONY: release test python python-test docker

LIB_DIR := lib/Linux-$(shell uname -m)
DOCKER_PLATFORM ?= linux/amd64,linux/arm64,linux/arm/v7

release:
	mkdir -p build
	cd build && PKG_CONFIG_PATH='../espeak-ng/build/lib/pkgconfig' cmake .. -DCMAKE_BUILD_TYPE=Release && make
	cp -a espeak-ng/build/lib/libespeak*.so* build/
	cp -R espeak-ng/build/share/espeak-ng-data build/
	cp -a $(LIB_DIR)/onnxruntime/lib/libonnxruntime*.so* build/
	cp etc/libtashkeel_model.ort build/

test:
	g++ -Wall -o test -Iespeak-ng/build/include -Lespeak-ng/build/lib -I$(LIB_DIR)/onnxruntime/include -L$(LIB_DIR)/onnxruntime/lib -Isrc -std=c++17 src/test.cpp src/phonemize.cpp src/phoneme_ids.cpp src/tashkeel.cpp -lespeak-ng -lonnxruntime
	LD_LIBRARY_PATH="espeak-ng/build/lib:$(LIB_DIR)/onnxruntime/lib" ./test 'espeak-ng/build/share/espeak-ng-data'

python:
	cp -R espeak-ng/build/share/espeak-ng-data piper_phonemize/
	cp etc/libtashkeel_model.ort piper_phonemize/
	LD_LIBRARY_PATH='espeak-ng/build/lib' .venv/bin/pip3 install -e .

python-test:
	LD_LIBRARY_PATH="espeak-ng/build/lib:$(LIB_DIR)/onnxruntime/lib" .venv/bin/python3 src/python_test.py

python-wheel:
	cp -R espeak-ng/build/share/espeak-ng-data piper_phonemize/
	cp etc/libtashkeel_model.ort piper_phonemize/
	LD_LIBRARY_PATH='espeak-ng/build/lib' .venv/bin/python3 setup.py bdist_wheel

docker:
	docker buildx build . --platform "$(DOCKER_PLATFORM)" --output 'type=local,dest=dist'
