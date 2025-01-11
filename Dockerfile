FROM debian:latest AS build
ARG TARGETARCH
ARG TARGETVARIANT
ENV LANG=C.UTF-8
ENV DEBIAN_FRONTEND=noninteractive

# system dependencies
RUN apt-get update && \
    apt-get install --yes --no-install-recommends \
    build-essential cmake ca-certificates curl pkg-config git

COPY --from=ghcr.io/astral-sh/uv:latest /uv /uvx /bin/

# build piper-phonemize
WORKDIR /build

COPY . .
RUN cmake -Bbuild -DCMAKE_INSTALL_PREFIX=install
RUN cmake --build build --config Release
RUN cmake --install build

# do a test run
RUN ./build/piper_phonemize --help

# build .tar.gz to keep symlinks
WORKDIR /dist
RUN mkdir -p piper_phonemize && \
    cp -dR /build/install/* ./piper_phonemize/ && \
    tar -czf "piper-phonemize_${TARGETARCH}${TARGETVARIANT}.tar.gz" piper_phonemize/

# build python wheel
ENV CPLUS_INCLUDE_PATH=/build/install/include
ENV LIBRARY_PATH=/build/install/lib

WORKDIR /build
ENV UV_PYTHON=3.13
RUN uv run setup.py sdist bdist_wheel
RUN cp dist/*.whl /dist/

# ---------------------

FROM scratch

COPY --from=build /dist/*.tar.gz ./
COPY --from=build /dist/*.whl ./
