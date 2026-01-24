FROM ubuntu:noble

SHELL ["/bin/bash", "-c"]

RUN apt-get update
# install geant4 requirements
RUN apt-get install -y \
cmake git gcc g++ libexpat1-dev qt6-base-dev \
libxmu-dev libmotif-dev

# set environment variables to configure install
ENV G4VERSION=11.4
ENV G4PATH=/software/geant4/

# install geant4
RUN git clone -b geant4-${G4VERSION}-release https://github.com/Geant4/geant4.git ${G4PATH}/geant4
WORKDIR software/geant4/geant4
RUN cmake -DGEANT4_INSTALL_DATA=ON \
-DCMAKE_INSTALL_PREFIX="${G4PATH}/geant4-install" \
-DGEANT4_USE_QT=ON -B build -S . && cmake --build build && cmake --install build

RUN echo "source ${G4PATH}/geant4-install/bin/geant4.sh" >> ~/.bashrc

COPY . /software/nwl

RUN source /software/geant4/geant4-install/bin/geant4.sh && \
cd /software/nwl && cmake -B build -S . && \
cmake --build build && cmake --install build

WORKDIR /

CMD ["/bin/bash"]