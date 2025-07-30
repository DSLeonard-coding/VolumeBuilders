#source this from a bash shell.
#DO NOT EDIT ORIGINAL
#Symlink it or copy it to ~/user_setup.sh   Copy if you need to edit it.


#Presently not used
#export VOLUME_BUILDER_GIT_DIR=~/repos/VolumeBuilder  

export Geant4_DIR="/opt/geant-10.7.4-C++17/geant4-install-4.10.7.4-C17-gcc-13.3.0/"
export ROOT_PATH="/opt/root/6.30.04-CPP17-install"
source ${Geant4_DIR}/bin/geant4.sh  # this should work on standard geant installs



####################### Probably don't need to edit below here:  ##################################################
export ROOT_DIR=${ROOT_PATH}
export ROOTSYS=${ROOT_DIR}
export GEANT4_PATH=${Geant4_DIR}

#Extra ROOT setup
source ${ROOT_PATH}/bin/thisroot.sh
export PATH=${VOLUME_BUILDER_GIT_DIR}/bin:${PATH}


unset MAKEFILES
