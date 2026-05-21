#ifndef SYSTEM_PLUGIN_MODEL_HH_
#define SYSTEM_PLUGIN_MODEL_HH_

#include <gz/sim/Model.hh>
#include <gz/sim/components/Name.hh>
#include <gz/sim/System.hh>
#include <gz/msgs/empty.pb.h>
#include <gz/transport/Node.hh>


namespace gz::sim::systems {
    class TopicControl :
            public System,
            public ISystemConfigure,
            public ISystemPreUpdate {
    public:
        TopicControl();

        ~TopicControl() override;

        //One time configuration of the plugin
        void Configure(const Entity &_entity,
                       const std::shared_ptr<const sdf::Element> &_sdf,
                       EntityComponentManager &_ecm,
                       EventManager &_eventMgr) override;

        //PreUpdate to change Model before physics timestep
        void PreUpdate(const UpdateInfo &_info, EntityComponentManager &_ecm) override;

    private:
        std::string modelName;
        std::string topicName;
        std::atomic<bool> reset{false};
        //This bool is atomic since gz-transport runs in a different thread https://gazebosim.org/docs/harmonic/architecture/
        std::atomic<bool> clearVelocity{false};
        Entity targetEntity{kNullEntity};
        transport::Node node;

        //If message gets Received this function gets called
        void OnTransportMsg(const msgs::Empty &_msg);
    };
} // namespace gz::sim::systems

#endif
