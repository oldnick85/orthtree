FROM ubuntu:25.10

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    # Version control
    git \
    # Formatting and static analysis
    clang-format \
    clang-tidy \
    libgtest-dev \
    doxygen graphviz \
    libgtkmm-4.0-dev \
    # Clean up to reduce image size
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Copy entrypoint script
COPY scripts/docker/linux/entrypoint.sh /opt/linux/entrypoint.sh
RUN chmod +x /opt/linux/entrypoint.sh

# Default entrypoint
ENTRYPOINT ["/opt/linux/entrypoint.sh"]