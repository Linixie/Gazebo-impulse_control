#ifndef GZ_SIM_SYSTEMS_IMPULSECONTROL_HH_
#define GZ_SIM_SYSTEMS_IMPULSECONTROL_HH_

#include <memory>
#include <gz/sim/System.hh>

namespace gz::sim::systems {
    class ImpulseControlPrivate;

    /// \brief A plugin that applies an impulse velocity and resets the rotation
            /// of the model it is attached to.
    class ImpulseControl
            : public System,
              public ISystemConfigure,
              public ISystemPreUpdate {
        /// \brief Constructor
    public:
        ImpulseControl();

        /// \brief Destructor
    public:
        ~ImpulseControl() override;

        // Documentation inherited
    public:
        void Configure(const Entity &_entity,
                       const std::shared_ptr<const sdf::Element> &_sdf,
                       EntityComponentManager &_ecm,
                       EventManager &_eventMgr) override;

        // Documentation inherited
    public:
        void PreUpdate(const UpdateInfo &_info,
                       EntityComponentManager &_ecm) override;

        /// \brief Private data pointer
    private:
        std::unique_ptr<ImpulseControlPrivate> dataPtr;
    };
}

#endif
