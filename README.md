# Gazebo Impulse Controller

Built for Continuous Reinforcement Learning (CRL), this Gazebo Harmonic plugin can be used to recover simulated robots from unsavable states. To avoid the time penalty of reloading a model over 50,000+ cycles, it uses topic messages to instantly execute a reload-free reset:

- Halts the model's movement.
- Resets its rotation.
- Applies momentary linear and angular velocities dynamically defined by the message contents.


## Usage

### 1) Add the plugin to your world (SDF)

Insert the plugin block anywhere inside the `<model>` element. Configure:

- `<topic_name>`: the topic name (`<topic>` is also accepted as an alias)

Example (resets the `box` model when a message is received on `/reset`):

```xml
<model name="box">
    <!-- Other model elements -->

    <plugin name="gz::sim::systems::ImpulseControl" filename="ImpulseControl">
        <topic_name>/reset</topic_name>
    </plugin>

    <!-- Other model elements -->
</model>
```

### 2) Publish a reset message

The plugin listens for `gz.msgs.Twist` messages and when a message arrives, it stops the model, resets its rotation, and (optionally) applies the linear/angular velocities provided in the message. IMPORTANT: If the links aren't static in the model the separate links will have the velocities applied causing them to, in the case of Angular Velocity, rotate on their own in the structure.

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
Note: It is not necessary to write it as a multiline command and it runs the same when collapsed into a single line command. Multiline was just used to make the command more readable.

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
Note: This is just a general outline of packages that I used while writing the plugin so some might not be necessary. 
<br>
There might also be some missing (In that case feel free to open an Issue so that I can fix the README).

## Installation

Clone the repository and build the project using just:

```bash
git clone https://github.com/Linixie/Gazebo-impulse_control.git
cd Gazebo-impulse_control/
just debug PATH/TO/FILE.sdf #(example: ./impulse_control/test.sdf for a simple test world)
```

### Running Manually (Without just)

If you prefer not to use the just command runner, or if you don't want to run Gazebo manually from the repository folder, you must add the plugins build directory to the `GZ_SIM_SYSTEM_PLUGIN_PATH` environment variable so Gazebo can find it:

```bash
cd PATH/TO/REPO/Gazebo-impulse_control/impulse_control/
mkdir build
cd build
cmake .. && make
export GZ_SIM_SYSTEM_PLUGIN_PATH=$(pwd):$GZ_SIM_SYSTEM_PLUGIN_PATH
gz sim PATH/TO/FILE.sdf
```

After following those steps you can just run gazebo from wherever you want and gazebo will find the plugin.
<br>
<br>
Note: When restarting the system or entering another shell this step has to be repeated as the variable is only set in the current shell instance.
<br>
To make the variable permanent add the export command with the full path to the build folder in your .bashrc or add the key=val pair into your environment file in /etc/environment.