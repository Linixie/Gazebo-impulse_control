default:
  just --list

#Build plugin
build:
    #!/usr/bin/env bash
    cd ./topic_controller/build
    cmake ..
    make

#Build plugin then start Gazebo with Verbosity
debug FILE: build
    #!/usr/bin/env bash
    cd ./topic_controller/
    export GZ_SIM_SYSTEM_PLUGIN_PATH=$(pwd)/build
    cd ../
    gz sim -v 4 {{FILE}}

#Start Gazebo without rebuilding plugin
start FILE:
    #!/usr/bin/env bash
    cd ./topic_controller/
    export GZ_SIM_SYSTEM_PLUGIN_PATH=$(pwd)/build:
    cd ../
    gz sim {{FILE}}
