FROM quay.io/pypa/manylinux_2_28_x86_64 as build-amd64

FROM quay.io/pypa/manylinux_2_28_aarch64 as build-arm64

ARG TARGETARCH
ARG TARGETVARIANT
FROM build-${TARGETARCH}${TARGETVARIANT} as build
ARG TARGETARCH
ARG TARGETVARIANT

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /build

# Build minimal version of espeak-ng
RUN git clone https://github.com/rhasspy/espeak-ng
RUN cd espeak-ng && \
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
COPY CMakeLists.txt ./
COPY src/ ./src/
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# Package libpiper_phonemize.so
RUN mkdir -p /dist/lib && \
    cd /dist && \
    cp /build/build/libpiper_phonemize.so ./lib/ && \
    find /usr -name 'libespeak-ng*.so*' -exec cp -a {} ./lib/ \; && \
    find /usr -type d -name 'espeak-ng-data' -exec cp -R {} ./lib/ \; && \
    mkdir -p ./include && \
    cp /build/src/phonemize.hpp /build/src/phoneme_ids.hpp ./include/ && \
    tar -czf piper_phonemize.tar.gz *

# Build piper_phonemize Python package
COPY setup.py pyproject.toml MANIFEST.in README.md LICENSE.md ./
COPY piper_phonemize/ ./piper_phonemize/
RUN find /usr -type d -name 'espeak-ng-data' -exec cp -R {} ./piper_phonemize/ \;
RUN /opt/python/cp39-cp39/bin/pip wheel .
RUN /opt/python/cp310-cp310/bin/pip wheel .
RUN /opt/python/cp311-cp311/bin/pip wheel .
RUN auditwheel repair *.whl

# -----------------------------------------------------------------------------

FROM scratch

COPY --from=build /dist/libpiper_phonemize.tar.gz ./
COPY --from=build /build/wheelhouse/ ./
