#List all Recipes
default:
  just --list

#Build plugin
build:
    #!/usr/bin/env bash
    cd ./impulse_control/build
    cmake ..
    make

#Build plugin then start Gazebo with Verbosity
debug FILE: build
    #!/usr/bin/env bash
    cd ./impulse_control/
    export GZ_SIM_SYSTEM_PLUGIN_PATH=$(pwd)/build:GZ_SIM_SYSTEM_PLUGIN_PATH
    cd ../
    gz sim -v 4 {{FILE}}

#Start Gazebo without rebuilding plugin
start FILE:
    #!/usr/bin/env bash
    cd ./impulse_control/
    export GZ_SIM_SYSTEM_PLUGIN_PATH=$(pwd)/build:GZ_SIM_SYSTEM_PLUGIN_PATH
    cd ../
    gz sim {{FILE}}
