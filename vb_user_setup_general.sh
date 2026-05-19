#source this from a bash shell.
#DO NOT EDIT ORIGINAL
#Symlink it or copy it to ~/vb_user_setup.sh   Copy if you need to edit it.


#Presently not used
#export VOLUME_BUILDER_GIT_DIR=~/repos/VolumeBuilder  

export GEANT4_PATH="/opt/geant4-11/geant4-install-11.4.1-CPP23-gcc-15.2.0-ROOT6.38.04/"
export ROOT_PATH="/opt/root/6.38.04-install-CPP23/"
#export Qt5_DIR="/user/lib/x86_64-linux-gnu/cmake/Qt5/"
source ${GEANT4_PATH}/bin/geant4.sh  # this should work on standard geant installs



####################### Probably don't need to edit below here:  ##################################################
export ASAN_OPTIONS=detect_leaks=1:color=always
export ROOT_DIR=${ROOT_PATH}
export ROOTSYS=${ROOT_DIR}
export GEANT4_PATH=${Geant4_DIR}

#Extra ROOT setup
source ${ROOT_PATH}/bin/thisroot.sh
export PATH=${VOLUME_BUILDER_GIT_DIR}/bin:${PATH}


unset MAKEFILES
