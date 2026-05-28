# Gazebo Topic Controller

Built for Continuous Reinforcement Learning (CRL), this Gazebo Harmonic plugin can be used to recover simulated robots from unsavable states caused by exaggerated world physics. To avoid the time penalty of reloading a model over 50,000+ cycles, it uses topic messages to instantly execute a reload-free reset:

- Halts the model's movement.
- Resets its rotation.
- Applies momentary linear and angular velocities dynamically defined by the message contents.


## Usage

### 1) Add the plugin to your world (SDF)

Insert the plugin block anywhere inside the `<world>` element. Configure:

- `<model_name>`: the name of the model you want to reset
- `<topic_name>`: the topic name 

Example (resets the `box` model when a message is received on `/reset`):

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

### 2) Publish a reset message

The plugin listens for `gz.msgs.Twist` messages and when a message arrives, it stops the model, resets its rotation, and (optionally) applies the linear/angular velocities provided in the message.

**Send a Twist (apply velocities):**

```bash
gz topic -t /reset -m gz.msgs.Twist -p '
linear {
  x: 0.0
  y: 0.0
  z: 0.0
}
angular {
  x: 0.0
  y: 0.0
  z: 0.0
}'
```

**Send an empty message (reset rotation only, no velocities applied):**

```bash
gz topic -t /reset -m gz.msgs.Twist -p ''
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
cd PATH/TO/REPO/Gazebo_Topic_Controller/topic_controller/
mkdir build
cd build
cmake .. && make
export GZ_SIM_SYSTEM_PLUGIN_PATH=$(pwd):$GZ_SIM_SYSTEM_PLUGIN_PATH
gz sim PATH/TO/FILE.sdf
```