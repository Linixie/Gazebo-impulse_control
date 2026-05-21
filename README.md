# Gazebo Topic Controller

A Gazebo Harmonic plugin that stops a specified model when a message is published to a designated topic.

## Usage

### Plugin Integration
To add the plugin to an existing `.sdf` file, include the following XML snippet inside your `<world>` tag.

In this example, the plugin stops the `box` model when a message is received on the `/reset` topic:

```xml
<world name="empty">
    <!-- Other world elements -->

    <plugin name="gz::sim::systems::TopicControl" filename="TopicControl">
        <model_name>box</model_name>
        <topic_name>reset</topic_name>
    </plugin>

    <!-- Other world elements -->
</world>
```

## Dependencies

Make sure you have the following dependencies installed:
* [Just](https://github.com/casey/just) (Command runner)
* [Gazebo Harmonic](https://github.com/gazebosim/gz-sim)

You will also need the Gazebo CLI tools. You can install Gazebo Harmonic and the required tools on Ubuntu using the following command:

```bash
sudo apt install gz-harmonic gz-launch7-cli gz-plugin2-cli gz-sim8-cli gz-tools2 gz-transport13-cli
```

## Installation

Clone the repository and build the project using just:

```bash
git clone https://github.com/Linixie/Gazebo_Topic_Controller.git
cd Gazebo_Topic_Controller/
just debug PATH/TO/FILE #(example: ./topic_controller/test.sdf for a simple test world)
```

### Running Manually (Without just)

If you prefer not to use the just script, or if you don't want to run Gazebo manually from the repository folder, you must add the plugins build directory to the `GZ_SIM_SYSTEM_PLUGIN_PATH` environment variable so Gazebo can find it:

```bash
export GZ_SIM_SYSTEM_PLUGIN_PATH=PATH/TO/REPO/Gazebo_Topic_Controller/topic_controller/build:$GZ_SIM_SYSTEM_PLUGIN_PATH
gz sim PATH/TO/FILE.sdf
```