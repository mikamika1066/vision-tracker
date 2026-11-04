#pragma once

#include "data_types.hpp"
#include <memory>
#include <vector>
#include "TrackingBase.hpp"

namespace rm {

class TargetManager {
public:
    Result onFrame(const InputFrame& frame);



private:
    std::vector<std::unique_ptr<TrackingBase>> targets_;

    TrackingBase* probeTarget(int id);

    bool has_successful_frame = false;

    int last_successful_frame=0;
    std::int64_t last_timestamp_ns = 0;
    std::int64_t last_successful_timestamp_ns=0;

    Result sendresulttarget(Result& targetresult);
};




}