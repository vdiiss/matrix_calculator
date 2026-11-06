# ---------- Этап 1: сборка ----------
FROM ubuntu:22.04 AS builder

RUN apt-get update \
    && apt-get install -y --no-install-recommends cmake g++ make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Тесты в образ не собираем: они выполняются в CI (job build-and-test)
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF \
    && cmake --build build --parallel

# ---------- Этап 2: минимальный образ для запуска ----------
FROM ubuntu:22.04

WORKDIR /app
COPY --from=builder /app/build/matrix_calculator .

# По умолчанию — демонстрационный режим;
# интерактивный: docker run -it --rm matrix-calculator ./matrix_calculator -i
CMD ["./matrix_calculator"]
