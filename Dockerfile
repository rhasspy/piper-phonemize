FROM quay.io/pypa/manylinux_2_28_x86_64 as build-amd64

FROM quay.io/pypa/manylinux_2_28_aarch64 as build-arm64

# -----------------------------------------------------------------------------

ARG TARGETARCH
ARG TARGETVARIANT
FROM build-${TARGETARCH}${TARGETVARIANT} as build
ARG TARGETARCH
ARG TARGETVARIANT

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /build

RUN mkdir -p "lib/Linux-$(uname -m)"

# Download and extract onnxruntime
ARG ONNXRUNTIME_VERSION='1.14.1'
RUN if [ "${TARGETARCH}${TARGETVARIANT}" = 'amd64' ]; then \
        ONNXRUNTIME_ARCH='x64'; \
    else \
        ONNXRUNTIME_ARCH="$(uname -m)"; \
    fi && \
    curl -L "https://github.com/microsoft/onnxruntime/releases/download/v${ONNXRUNTIME_VERSION}/onnxruntime-linux-${ONNXRUNTIME_ARCH}-${ONNXRUNTIME_VERSION}.tgz" | \
        tar -C "lib/Linux-$(uname -m)" -xzvf - && \
    mv "lib/Linux-$(uname -m)"/onnxruntime-* \
       "lib/Linux-$(uname -m)/onnxruntime"

# Build minimal version of espeak-ng
RUN curl -L "https://github.com/rhasspy/espeak-ng/archive/refs/heads/master.tar.gz" | \
    tar -xzvf - && \
    cd espeak-ng-master && \
    ./autogen.sh && \
    ./configure \
        --without-pcaudiolib \
        --without-klatt \
        --without-speechplayer \
        --without-mbrola \
        --without-sonic \
        --with-extdict-cmn \
        --prefix=/usr && \
    make -j8 src/espeak-ng src/speak-ng && \
    make && \
    make install

# Build libpiper_phonemize.so
COPY etc/libtashkeel_model.ort ./etc/
COPY CMakeLists.txt Makefile ./
COPY src/ ./src/

# Sanity check
RUN make test

# Build libpiper_phonemize.so
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

# Package libpiper_phonemize.so
RUN mkdir -p /dist/lib && \
    cd /dist && \
    cp /build/build/libpiper_phonemize.so ./lib/ && \
    find /usr -name 'libespeak-ng*.so*' -exec cp -a {} ./lib/ \; && \
    find /usr -type d -name 'espeak-ng-data' -exec cp -R {} ./lib/ \; && \
    mkdir -p ./include && \
    cp -R /usr/include/espeak-ng ./include/ && \
    cp /build/src/phonemize.hpp /build/src/phoneme_ids.hpp /build/src/tashkeel.hpp ./include/ && \
    cp -a "/build/lib/Linux-$(uname -m)/onnxruntime/lib"/libonnxruntime*.so* ./lib/ && \
    cp -R "/build/lib/Linux-$(uname -m)/onnxruntime/include"/* ./include/ && \
    cp -R /build/etc ./ && \
    tar -czf libpiper_phonemize.tar.gz *

# Build piper_phonemize Python package
# COPY setup.py pyproject.toml MANIFEST.in README.md LICENSE.md ./
# COPY piper_phonemize/ ./piper_phonemize/
# RUN find /usr -type d -name 'espeak-ng-data' -exec cp -R {} ./piper_phonemize/ \;
# RUN /opt/python/cp39-cp39/bin/pip wheel .
# RUN /opt/python/cp310-cp310/bin/pip wheel .
# RUN /opt/python/cp311-cp311/bin/pip wheel .
# RUN auditwheel repair *.whl

# -----------------------------------------------------------------------------

FROM scratch
ARG TARGETARCH
ARG TARGETVARIANT

COPY --from=build /dist/libpiper_phonemize.tar.gz ./libpiper_phonemize-${TARGETARCH}${TARGETVARIANT}.tar.gz
# COPY --from=build /build/wheelhouse/ ./
