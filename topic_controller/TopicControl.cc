#include "TopicControl.hh"

#include <gz/sim/components/Pose.hh>
#include <gz/sim/components/LinearVelocityCmd.hh>
#include <gz/sim/components/AngularVelocityCmd.hh>
#include <gz/common/Console.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Quaternion.hh>
#include <gz/plugin/Register.hh>

using namespace gz;
using namespace sim;
using namespace systems;

//Constructor and Deconstructor
TopicControl::TopicControl() { gzdbg << "Plugin Started" << std::endl; }

TopicControl::~TopicControl() { gzdbg << "Plugin Stopped" << std::endl; }

//Plugin Configuration
//Only called Once
void TopicControl::Configure(const gz::sim::Entity &_entity,
                             const std::shared_ptr<const sdf::Element> &_sdf,
                             gz::sim::EntityComponentManager &_ecm,
                             gz::sim::EventManager & /*_eventMgr*/) {
    //Read Target Model_Name from .sdf file
    if (!_sdf->HasElement("model_name")) {
        gzerr << "MoveModel plugin requires a <model_name> element." << std::endl;
        return;
    }


    this->modelName = _sdf->Get<std::string>("model_name");
    gzmsg << "Target Model Name: " << this->modelName << std::endl;


    //Read Target Topic_Name from .sdf file
    if (!_sdf->HasElement("topic_name")) {
        gzerr << "MoveModel plugin requires a <topic_name> element." << std::endl;
        return;
    }


    this->topicName = _sdf->Get<std::string>("topic_name");
    gzmsg << "Topic Name: " << this->topicName << std::endl;


    //Start Listening for Messages
    this->node.Subscribe(this->topicName, &TopicControl::OnTransportMsg, this);
}


//When receiving Message save Boolean in this->reset
void TopicControl::OnTransportMsg(const gz::msgs::Empty &_msg) {
    this->reset.store(true);
}


void TopicControl::PreUpdate(const gz::sim::UpdateInfo &_info,
                             gz::sim::EntityComponentManager &_ecm) {
    //Set this->targetEntity once
    if (this->targetEntity == kNullEntity) {
        const auto entityOpt = _ecm.EntityByName(this->modelName);
        if (!entityOpt.has_value()) {
            gzdbg << "Model [" << this->modelName
                    << "] not found yet. Skipping velocity application." << std::endl;
            return;
        }

        this->targetEntity = entityOpt.value();
        gzmsg << "Found target model entity: " << this->targetEntity << std::endl;
    }


    //Remove VelocityCmd Components
    //https://github.com/gazebosim/gz-sim/issues/1926
    if (this->clearVelocity.exchange(false)) {
        const gz::sim::Model model(this->targetEntity);
        const auto links = model.Links(_ecm);
        for (const auto &linkEntity: links) {
            _ecm.RemoveComponent<gz::sim::components::LinearVelocityCmd>(linkEntity);
            _ecm.RemoveComponent<gz::sim::components::AngularVelocityCmd>(linkEntity);
        }
    }


    //Return if reset is not set
    if (!this->reset.exchange(false)) {
        return;
    }


    // Get current pose
    const auto poseComp = _ecm.Component<gz::sim::components::Pose>(this->targetEntity);


    if (!poseComp) {
        gzdbg << "Pose component not found" << std::endl;
        return;
    }


    gz::math::Pose3d newPose = poseComp->Data();


    newPose.Rot() = gz::math::Quaterniond::Identity;


    gz::sim::Model model(this->targetEntity);


    model.SetWorldPoseCmd(_ecm, newPose);


    //Apply zero vector to all links
    const auto links = model.Links(_ecm);
    for (const auto &linkEntity: links) {
        _ecm.SetComponentData<gz::sim::components::LinearVelocityCmd>(
            linkEntity,
            gz::math::Vector3d::Zero
        );

        _ecm.SetComponentData<gz::sim::components::AngularVelocityCmd>(
            linkEntity,
            gz::math::Vector3d::Zero
        );
    }


    this->clearVelocity.store(true);


    gzdbg << "Pose and velocities reset" << std::endl;
}

GZ_ADD_PLUGIN(gz::sim::systems::TopicControl, gz::sim::System,
              gz::sim::ISystemConfigure, gz::sim::ISystemPreUpdate);

GZ_ADD_PLUGIN_ALIAS(gz::sim::systems::TopicControl,
                    "gz::sim::systems::TopicControl")
