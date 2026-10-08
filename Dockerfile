# Dockerfile -- Geant4 11.2.2 (Qt + GDML + all physics data) on Ubuntu 24.04
# Build:   docker build -t g4gd:11.2.2 .
# Requires internet during build (downloads Geant4 source + data sets).
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV G4INSTALL=/opt/geant4
ENV PATH=$G4INSTALL/bin:$PATH
ENV LD_LIBRARY_PATH=$G4INSTALL/lib:$LD_LIBRARY_PATH

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake wget git ca-certificates \
        libxerces-c-dev libexpat1-dev \
        qtbase5-dev qtbase5-dev-tools libqt5opengl5-dev \
        mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev \
        tcsh \
    && cd /opt \
    && G4_VERSION=11.2.2 \
    && wget https://github.com/Geant4/geant4/archive/refs/tags/v${G4_VERSION}.tar.gz \
    && tar -xf v${G4_VERSION}.tar.gz \
    && cmake -S geant4-${G4_VERSION} -B geant4-build \
        -DCMAKE_INSTALL_PREFIX=$G4INSTALL \
        -DGEANT4_BUILD_MULTITHREADED=ON \
        -DGEANT4_USE_GDML=ON \
        -DGEANT4_USE_SYSTEM_EXPAT=ON \
        -DGEANT4_USE_SYSTEM_XERCESC=ON \
        -DGEANT4_INSTALL_DATA=ON \
        -DGEANT4_INSTALL_EXAMPLES=OFF \
        -DGEANT4_USE_QT=ON \
        -DGEANT4_USE_OPENGL_X11=OFF \
        -DGEANT4_USE_XM=OFF \
        -DGEANT4_USE_RAYTRACER_X11=OFF \
    && cmake --build geant4-build --target install -j$(nproc) \
    && rm -rf /opt/v${G4_VERSION}.tar.gz /opt/geant4-${G4_VERSION} /opt/geant4-build \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
CMD ["bash"]
