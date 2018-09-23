#pragma once

#include <Scheduler/Common/Clock.h>
#include <Scheduler/Lib/UUID.h>
#include <iosfwd>
#include <memory>
#include <vector>

namespace Scheduler {
namespace Lib {

    class Scheduler;

    enum TaskState
    {
        NEW = 0,
        SUCCESS = 1,
        PENDING,
        ACTIVE,
        FAILED,
        CANCELLED
    };

    const char* TaskStateToStr(TaskState state);

    std::ostream& operator<<(std::ostream& o, TaskState state);

    class Task;
    typedef std::shared_ptr<Task> TaskPtr;

    class Task : public std::enable_shared_from_this<Task>
    {
        friend class Scheduler;

    public:
        /// Create a task that should not execute after a given time.
        /// It is expected that the time point given be in the future and
        /// creating one in the past will generate a bad task.
        static TaskPtr After(const Clock::time_point& point);

        /// Create a Task that should not execute before a given time.
        /// It is expected that the time point given be in the future and
        /// creating one in the past will generate a bad task.
        static TaskPtr Before(const Clock::time_point& point);

        /// Create a task that should execute between two given time
        /// points. Both values should follow the rules for the
        /// individual constructors in be in the future or now.
        static TaskPtr Between(
            const Clock::time_point& after,
            const Clock::time_point& before);

        /// Create a simple task that has no time boundaries for execution.
        static TaskPtr Create();

        /// The destructor.
        ~Task();

        /// Add the given task a dependency that must complete executing before
        /// this task may begin. ANy changes to the dependency list may be
        /// chained however you should always check IsValid before proceeding to
        /// ensure the dependency chain is a valid run-path.
        Task* Depends(Task* task);

        /// Add the given task a dependency that must complete executing before
        /// this task may begin. ANy changes to the dependency list may be
        /// chained however you should always check IsValid before proceeding to
        /// ensure the dependency chain is a valid run-path.
        inline Task* Depends(TaskPtr& task) { return Depends(task.get()); }

        /// Retrieve teh state for the task.
        TaskState GetState() const { return m_state; }

        /// Predicate check if the task has dependencies set.
        bool HasDependencies() const { return !m_dependencies.empty(); }

        /// Retrieve the ID for the task.
        const UUID& Id() const { return m_id; }

        /// Predeicate to check if the task is currently executing. This
        /// could be multiple states, but it if true it always means the
        /// task can no longer be modified.
        bool IsActive() const;

        /// Predicate check if the task has completed. This should not be
        /// used to determine success or failure, just that the task is now
        /// complete.
        bool IsComplete() const;

        /// Check if the task has expired based on the time range
        /// given during construction.
        bool IsExpired() const;

        /// Check if the task is premature and not yet ready to run based
        /// on the given time range during construction.
        bool IsPremature() const;

        /// Predicate check if the task is valid. This flag could get set
        /// during construction or anytime a dependency is added if that
        /// dependency would cause the task to never complete.
        bool IsValid() const;

        /// Check if the given task is a required dependency for this
        // task to run.
        bool Requires(const Task* task) const;

        /// Check if the given task is a required dependency for this
        // task to run.
        bool Requires(const TaskPtr& task) const;

        /// Check if the given task is a required dependency for this
        // task to run.
        bool Requires(const UUID& id) const;

        /// Retrieve the Task identifier as a string or with a descriptive
        /// identifier.
        std::string ToString(bool asShort = false) const;

    protected:
        Task();
        Task(const Clock::time_point& after, const Clock::time_point& before);

    private:
        void SetState(TaskState state);

        UUID m_id;
        TaskState m_state;

        Clock::time_point m_createdOn;
        Clock::time_point m_before;
        Clock::time_point m_after;
        bool m_valid = true;

        std::vector<TaskPtr> m_dependencies;
    };

    std::ostream& operator<<(std::ostream& o, const Task* task);

    std::ostream& operator<<(std::ostream& o, const TaskPtr& task);

}  // namespace Lib
}  // nmaespace Scheduler