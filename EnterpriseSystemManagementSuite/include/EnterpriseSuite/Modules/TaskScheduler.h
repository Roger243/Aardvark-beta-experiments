#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct ScheduledTask {
    std::string task_id;
    std::string description;
    std::chrono::system_clock::time_point next_run_time;
};

class ITaskScheduler {
public:
    virtual ~ITaskScheduler() = default;

    virtual Common::OperationResult RegisterTask(const ScheduledTask& task) = 0;
    virtual Common::OperationResult RemoveTask(const std::string& task_id) = 0;
    virtual Common::OperationResult RunTaskNow(const std::string& task_id) = 0;
    virtual std::vector<ScheduledTask> ListTasks() const = 0;
};

}  // namespace EnterpriseSuite::Modules
