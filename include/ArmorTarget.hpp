#pragma once

#include "TrackingBase.hpp"

namespace rm {

class ArmorTarget final : public TrackingBase {
public:
    ArmorTarget(const Observation& observation, std::int64_t observed_time_ns)
        :TrackingBase(observation, TargetType::ARMOR, observed_time_ns) {}
        

        int lostThreshold() const override {
            return 3;
        }

private:
        
};

}