#include "TrackingBase.hpp"

namespace rm{

/*
现在逻辑是将遍历vector里的id拿去和进来的frame数据作比较
如果miss,如果没有miss,然后再判断是否为SPINNING决定是否计算角速度
*/
    void TrackingBase::update(const Observation* observation, const InputFrame& frame, double dt) {
        TargetStatus old_status = status_;

        if (observation != nullptr && status_ == TargetStatus::TRACK) {                                         ///为了防止在LOST状态下继续按照不存在的x.y及time_stum生成错误的速度，在这里必须判断observation是否传回了坐标
            missed_count_ = 0;
            double dt_obs = (frame.timestamp_ns - last_observed_time_ns_) / 1e9;   //计算dt_obs
            last_observed_time_ns_ = frame.timestamp_ns;
            velocity_x_ = (observation->x - x_) / dt_obs;
            velocity_y_ = (observation->y - y_) / dt_obs;
            if (type_ == TargetType::SPINNING) {
                angular_velocity_ = (observation->angle - angle_) / dt_obs;
                angle_ = observation->angle;
            }
            x_ = observation->x;
            y_ = observation->y;
        }

        if (old_status == TargetStatus::LOST && observation != nullptr) {
                status_ = TargetStatus::TRACK;
                velocity_x_ = 0;
                velocity_y_ = 0;
                x_ = observation->x;
                y_ = observation->y;
                predicted_x_ = x_;
                predicted_y_ = y_;
                missed_count_ = 0;
                last_observed_time_ns_ = frame.timestamp_ns;
                if (type_ == TargetType::SPINNING) {
                    angular_velocity_ = 0;
                    angle_ = observation->angle;
                }
                predicted_angle_ = angle_;
        }

        if (observation == nullptr && status_ == TargetStatus::TRACK) {      //这里采用指针和TargetStatus联合判断，若为TRACK且指针不为空，则说明观测到了目标，若为TRACK但指针为空，则说明未观测到目标
            missed_count_++;
        }

        if (missed_count_ < lostThreshold()) {
            if (missed_count_ == 0) {
                predicted_x_ = x_ + velocity_x_ * dt;
                predicted_y_ = y_ + velocity_y_ * dt;
            } else {
                predicted_x_ += velocity_x_ * dt;
                predicted_y_ += velocity_y_ * dt;
            }
        } else {
            x_ = 0;
            y_ = 0;
            predicted_x_ = 0;
            predicted_y_ = 0;
            velocity_x_ = 0;
            velocity_y_ = 0;
            status_ = TargetStatus::LOST;
        }

        if (type_ == TargetType::SPINNING) {
            if (missed_count_ < lostThreshold()) {
                if (missed_count_ == 0) {
                    predicted_angle_ = angle_ + angular_velocity_ * dt;
                } else {
                    angle_ += angular_velocity_ * dt;
                    predicted_angle_ = angle_ + angular_velocity_ * dt;
                }
            } else {
                angle_ = 0;
                angular_velocity_ = 0;
                predicted_angle_ = 0;
            }

        }

    }

}













