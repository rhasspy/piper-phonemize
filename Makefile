.PHONY: release release-windows test python python-test docker docker-windows

LIB_DIR := lib/Linux-$(shell uname -m)
DOCKER_PLATFORM ?= linux/amd64,linux/arm64,linux/arm/v7
VENV ?= .venv

release:
	mkdir -p build
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make
	cp -a espeak-ng/build/lib/libespeak*.so* build/
	cp -R espeak-ng/build/share/espeak-ng-data build/
	cp -a $(LIB_DIR)/onnxruntime/lib/libonnxruntime*.so* build/
	cp etc/libtashkeel_model.ort build/

release-windows:
	mkdir -p build
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make
	cp -a espeak-ng/build_windows/lib/libespeak*.dll build/
	cp -R espeak-ng/build/share/espeak-ng-data build/
	cp -a $(LIB_DIR)/onnxruntime/lib/libonnxruntime*.dll build/
	cp etc/libtashkeel_model.ort build/

test:
	g++ -Wall -o test -Iespeak-ng/build/include -Lespeak-ng/build/lib -I$(LIB_DIR)/onnxruntime/include -L$(LIB_DIR)/onnxruntime/lib -Isrc -std=c++17 src/test.cpp src/phonemize.cpp src/phoneme_ids.cpp src/tashkeel.cpp src/shared.cpp -lespeak-ng -lonnxruntime
	LD_LIBRARY_PATH="espeak-ng/build/lib:$(LIB_DIR)/onnxruntime/lib:${LD_LIBRARY_PATH}" ./test 'espeak-ng/build/share/espeak-ng-data'

python:
	cp -R espeak-ng/build/share/espeak-ng-data piper_phonemize/
	cp etc/libtashkeel_model.ort piper_phonemize/
	LD_LIBRARY_PATH="espeak-ng/build/lib:${LD_LIBRARY_PATH}" "$(VENV)/bin/pip3" install -e .

python-test:
	LD_LIBRARY_PATH="espeak-ng/build/lib:$(LIB_DIR)/onnxruntime/lib:${LD_LIBRARY_PATH}" "$(VENV)/bin/python3" src/python_test.py

python-wheel:
	cp -R espeak-ng/build/share/espeak-ng-data piper_phonemize/
	cp etc/libtashkeel_model.ort piper_phonemize/
	LD_LIBRARY_PATH="espeak-ng/build/lib:${LD_LIBRARY_PATH}" "$(VENV)/bin/python3" setup.py bdist_wheel

docker:
	docker buildx build . --platform "$(DOCKER_PLATFORM)" --output 'type=local,dest=dist'

docker-windows:
	docker buildx build . -f Dockerfile.windows --output 'type=local,dest=dist/windows_amd64'
