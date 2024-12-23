FROM ubuntu:latest

RUN apt update -qq && \
    apt install -y -qq \
        gcc \
        vim \
        man \
        make \
        net-tools && \
    rm -rf /var/lib/apt/lists/*  # Clean up to reduce image size

CMD ["/bin/bash"]
