FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    cmake \
    python3 \
    valgrind

WORKDIR /app

COPY . .

RUN make clean && make

CMD ["python3", "tests/run_tests.py"]
