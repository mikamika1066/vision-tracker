#pragma once

#include <cstdint>
#include "data_types.hpp"

namespace rm {

class TrackingBase {
public:
    TrackingBase(const Observation& observation, TargetType type, std::int64_t observed_time_ns)
        :id_(observation.id),
        type_(type),
        last_observed_time_ns_(observed_time_ns),
        status_(TargetStatus::TRACK),
        x_(observation.x),
        y_(observation.y),
        velocity_x_(0.0),
        velocity_y_(0.0),
        predicted_x_(observation.x),
        predicted_y_(observation.y),
        angle_(type==TargetType::SPINNING ? observation.angle : 0),
        angular_velocity_(0.0),
        predicted_angle_(type==TargetType::SPINNING ? observation.angle : 0),
        missed_count_(0) {

    }

    virtual ~TrackingBase() =default;

    int id() const {
        return id_;
    }

    TargetType type() const {
        return type_;
    }

    TargetStatus status() const {
        return status_;
    }

    TargetSnapshot snapshot() const {
        TargetSnapshot result{};

        result.id = id_;
        result.type = type_;
        result.status = status_;
        result.x = x_;
        result.y = y_;
        result.velocity_x = velocity_x_;
        result.velocity_y = velocity_y_;
        result.predicted_x = predicted_x_;
        result.predicted_y = predicted_y_;
        result.angle = angle_;
        result.angular_velocity = angular_velocity_;
        result.predicted_angle = predicted_angle_;
        result.missed_count = missed_count_;

        return result;

    }

    virtual int lostThreshold() const = 0;
    
    /* 调用update函数时会对一个目标的observation进行读取，然后用其数据计算x,y,velocity,pridict_x
    原理是外部代码无法直接访问private的变量，但利用类里的Public函数可以访问同类的private来间接访问
    */
    void update(const Observation* observation, const InputFrame& frame, double dt);

private:
    int id_;
    TargetType type_;
    std::int64_t last_observed_time_ns_;
    TargetStatus status_;
    double x_;
    double y_;
    double velocity_x_;
    double velocity_y_;
    double predicted_x_;
    double predicted_y_;
    double angle_;
    double angular_velocity_;
    double predicted_angle_;
    int missed_count_;



};

}