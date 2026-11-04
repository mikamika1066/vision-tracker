#pragma once

#include "TrackingBase.hpp"

namespace rm {

class SpinningTarget final : public TrackingBase {
public:
    SpinningTarget(const Observation& observation, std::int64_t observed_time_ns)
        :TrackingBase(observation, TargetType::SPINNING, observed_time_ns) {
        }
        
        

        int lostThreshold() const override {
            return 5;
        }
};

}

