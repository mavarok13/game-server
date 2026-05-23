FROM gcc:11.3-bullseye AS build

WORKDIR /app

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        cmake \
        python3-pip \
        ca-certificates && \
    pip3 install --no-cache-dir "conan<2" && \
    rm -rf /var/lib/apt/lists/*

COPY conanfile.txt ./
RUN conan profile new default --detect && \
    conan profile update settings.compiler.libcxx=libstdc++11 default && \
    mkdir build && \
    conan install . \
        --install-folder=build \
        --build=missing \
        -s build_type=Release

COPY CMakeLists.txt ./
COPY src ./src
COPY tests ./tests

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release --target game_server --parallel

RUN find /app/build -maxdepth 4 -type f -executable -print

FROM debian:bookworm-slim AS run

WORKDIR /app

RUN apt-get update && \
    apt-get install -y --no-install-recommends libstdc++6 libgcc-s1 libpq5 ca-certificates && \
    rm -rf /var/lib/apt/lists/* && \
    useradd --create-home --shell /usr/sbin/nologin appuser && \
    mkdir -p /app/game_server_saves && \
    chown -R appuser:appuser /app

COPY --from=build /app/build/game_server ./game_server
COPY data ./data
COPY static ./static

USER appuser

EXPOSE 8080

ENTRYPOINT ["/app/game_server"]
CMD ["-c", "/app/data/config.json", "-w", "/app/static", "--tick-period", "100", "--state-file", "/app/game_server_saves/state.dat"]
