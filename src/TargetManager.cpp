#include "TargetManager.hpp"
#include "TrackingBase.hpp"
#include "ArmorTarget.hpp"
#include "SpinningTarget.hpp"

namespace rm {

    TrackingBase* TargetManager::probeTarget(int id)
    {
        for (int i = 0; i < targets_.size(); i++) {
            if (targets_[i]->id()==id) {
                return targets_[i].get();
            }
        }

        return nullptr;

    }

    Result TargetManager::sendresulttarget(Result& targetresult)
    {
        for (int i = 0; i < targets_.size(); i++) {
            TargetSnapshot outputsnapshot = targets_[i]->snapshot();
            targetresult.targets.push_back(outputsnapshot);

            if (outputsnapshot.type == TargetType::ARMOR) {
                targetresult.armor_target_count++;
            } else {
                targetresult.spinning_target_count++;
            }

            if (outputsnapshot.status == TargetStatus::TRACK) {
                targetresult.tracking_target_count++;
            } else {
                targetresult.lost_target_count++;
            }
        }

        return targetresult;
    }


    Result TargetManager::onFrame(const InputFrame& frame) {
        bool isillegal = false;                                              ///声明需反复使用的变量
        double dt = 0;
        TrackingBase* existing_target = nullptr;
        std::int64_t t_ns = frame.timestamp_ns;

        for (int i = 0; i < frame.observations.size(); i++) {
                for (int j = i+1; j < frame.observations.size(); j++ ) {
                    if (frame.observations[i].id == frame.observations[j].id) {
                        isillegal = true;
                        break;
                    }
                }

        if (frame.observations[i].id < 0) {
            isillegal = true;
        }

        for (int j = 0; j < frame.observations.size(); j++) {
            existing_target =probeTarget(frame.observations[j].id);

            if (existing_target != nullptr && existing_target->type() != frame.observations[j].type) {
                isillegal = true;
            }

        }

        }

        if (isillegal == false) {
            if (has_successful_frame == false) {
                has_successful_frame = true;
                isillegal = false;
                last_successful_frame = frame.frame_id;
                last_successful_timestamp_ns = frame.timestamp_ns;
            } else {
                if (frame.frame_id > last_successful_frame && frame.timestamp_ns > last_successful_timestamp_ns) {
                    isillegal = false;
                    double t_prev_ns = last_successful_timestamp_ns;
                    t_ns = frame.timestamp_ns;
                    dt = (t_ns - t_prev_ns) / 1e9;   //计算dt
                    last_successful_timestamp_ns = frame.timestamp_ns;
                    last_successful_frame = frame.frame_id;
                } else {
                    isillegal = true;
                }
            }
        }

        Result targetresult{};

        if (isillegal == true) {
            targetresult.accepted = false;
            targetresult.frame_id = frame.frame_id;
        } else {
            targetresult.accepted = true;
            targetresult.frame_id = frame.frame_id;
            for (int i = 0; i < frame.observations.size(); i++) {       ///先遍历接收到的observation，拿每一个目标与已知的targets_进行比较
                bool has_seen = false;                                  /// ，若存在则更新其状态，若不存在则新建一个目标
                for (int j = 0; j < targets_.size(); j++) {
                    if (targets_[j]->id() == frame.observations[i].id) {
                        existing_target = probeTarget(frame.observations[i].id);
                        existing_target->update(&frame.observations[i], frame, dt);
                        has_seen = true;
                        break;
                    }

                }
                if (has_seen == false) {
                    if (frame.observations[i].type == TargetType::ARMOR) {
                        targets_.push_back(std::make_unique<ArmorTarget>(frame.observations[i], frame.timestamp_ns));
                    } else {
                        targets_.push_back(std::make_unique<SpinningTarget>(frame.observations[i], frame.timestamp_ns));
                    }
                }
            }

            for (int i = 0; i < targets_.size(); i++) {
                bool has_found = false;                            //放进内层循环，对与每个已知的target初始化一次bool
                for (int j = 0; j < frame.observations.size(); j++) {
                    if (targets_[i]->id() == frame.observations[j].id) {
                        has_found = true;
                        break;
                    }

                }

                if (has_found == false) {
                    targets_[i]->update(nullptr, frame, dt);
                }

            }

        }

        sendresulttarget(targetresult);
        return targetresult;

    }

}