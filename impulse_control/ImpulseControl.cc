#include "ImpulseControl.hh"

#include <mutex>
#include <string>

#include <gz/common/Console.hh>
#include <gz/math/Pose3.hh>
#include <gz/math/Quaternion.hh>
#include <gz/msgs/twist.pb.h>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>
#include <gz/sim/Model.hh>
#include <gz/sim/Util.hh>
#include <gz/sim/components/AngularVelocityCmd.hh>
#include <gz/sim/components/LinearVelocityCmd.hh>
#include <gz/sim/components/Pose.hh>
#include <gz/transport/Node.hh>

using namespace gz;
using namespace sim;
using namespace systems;

namespace gz::sim::systems {
    class ImpulseControlPrivate {
        /// \brief Callback for model Twist subscription
    /// \param[in] _msg Twist message
    public:
        void OnTransportMsg(const msgs::Twist &_msg);

        /// \brief Gazebo communication node.
    public:
        transport::Node node;

        /// \brief Model interface
    public:
        Model model{kNullEntity};

        /// \brief Last target velocity requested.
    public:
        msgs::Twist targetVel;

        /// \brief Mutex to protect shared state between transport thread and PreUpdate
    public:
        std::mutex msgMutex;

        /// \brief Flag to indicate a new velocity message was received
    public:
        bool reset{false};

        /// \brief Flag to schedule the clearing of velocities on the next step
    public:
        bool clearVelocity{false};
    };

    //////////////////////////////////////////////////
    ImpulseControl::ImpulseControl()
        : dataPtr(std::make_unique<ImpulseControlPrivate>()) {
    }

    //////////////////////////////////////////////////
    ImpulseControl::~ImpulseControl() = default;

    //////////////////////////////////////////////////
    void ImpulseControl::Configure(const Entity &_entity,
                                   const std::shared_ptr<const sdf::Element> &_sdf,
                                   EntityComponentManager &_ecm,
                                   EventManager & /*_eventMgr*/) {
        this->dataPtr->model = Model(_entity);

        if (!this->dataPtr->model.Valid(_ecm)) {
            gzerr << "ImpulseControl plugin should be attached to a model entity. "
                    << "Failed to initialize." << '\n';
            return;
        }

        // Read Target Topic_Name from .sdf file
        std::string topicName;
        if (_sdf->HasElement("topic_name")) {
            topicName = _sdf->Get<std::string>("topic_name");
        } else if (_sdf->HasElement("topic")) {
            topicName = _sdf->Get<std::string>("topic");
        } else {
            gzerr << "ImpulseControl plugin requires a <topic_name> or <topic> element." << '\n';
            return;
        }

        gzmsg << "ImpulseControl subscribing to Twist messages on [" << topicName << "]" << '\n';

        // Start Listening for Messages
        this->dataPtr->node.Subscribe(topicName, &ImpulseControlPrivate::OnTransportMsg,
                                      this->dataPtr.get());
    }

    //////////////////////////////////////////////////
    void ImpulseControlPrivate::OnTransportMsg(const msgs::Twist &_msg) {
        std::scoped_lock lock(this->msgMutex);
        this->targetVel = _msg;
        this->reset = true;
    }

    //////////////////////////////////////////////////
    void ImpulseControl::PreUpdate(const UpdateInfo &_info,
                                   EntityComponentManager &_ecm) {
        if (_info.paused)
            return;

        // 1. Remove previously applied VelocityCmd Components if requested
        bool shouldClear = false;
        {
            std::scoped_lock lock(this->dataPtr->msgMutex);
            if (this->dataPtr->clearVelocity) {
                shouldClear = true;
                this->dataPtr->clearVelocity = false;
            }
        }

        if (shouldClear) {
            const auto links = this->dataPtr->model.Links(_ecm);
            for (const auto &linkEntity: links) {
                _ecm.RemoveComponent<components::LinearVelocityCmd>(linkEntity);
                _ecm.RemoveComponent<components::AngularVelocityCmd>(linkEntity);
            }
        }

        // 2. Safely grab the latest message data
        msgs::Twist velMsg;
        bool hasNewMsg = false;

        {
            std::scoped_lock lock(this->dataPtr->msgMutex);
            if (this->dataPtr->reset) {
                velMsg = this->dataPtr->targetVel;
                hasNewMsg = true;
                this->dataPtr->reset = false;
            }
        }

        if (!hasNewMsg)
            return;

        // 3. Get current pose and remove rotations
        const auto poseComp = _ecm.Component<components::Pose>(this->dataPtr->model.Entity());

        if (!poseComp) {
            gzdbg << "Pose component not found for model ["
                    << this->dataPtr->model.Name(_ecm) << "]" << '\n';
            return;
        }

        math::Pose3d newPose = poseComp->Data();
        newPose.Rot() = math::Quaterniond::Identity;
        this->dataPtr->model.SetWorldPoseCmd(_ecm, newPose);

        // 4. Apply vector to all links
        const auto links = this->dataPtr->model.Links(_ecm);
        for (const auto &linkEntity: links) {
            _ecm.SetComponentData<components::LinearVelocityCmd>(
                linkEntity,
                msgs::Convert(velMsg.linear())
            );

            _ecm.SetComponentData<components::AngularVelocityCmd>(
                linkEntity,
                msgs::Convert(velMsg.angular())
            );
        }

        // 5. Trigger clearing for the next frame
        {
            std::scoped_lock lock(this->dataPtr->msgMutex);
            this->dataPtr->clearVelocity = true;
        }

        gzdbg << "Pose and velocities reset for model ["
                << this->dataPtr->model.Name(_ecm) << "]" << '\n';
    }
}
GZ_ADD_PLUGIN(ImpulseControl,
              System,
              ImpulseControl::ISystemConfigure,
              ImpulseControl::ISystemPreUpdate)

GZ_ADD_PLUGIN_ALIAS(ImpulseControl, "gz::sim::systems::ImpulseControl")
