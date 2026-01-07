FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /watchdog

COPY watchdog.h watchdog.c ./
COPY tests/ ./tests/
COPY Makefile ./

RUN make

CMD ["python3", "tests/test_runner.py"]
