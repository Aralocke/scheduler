#include <gtest/gtest.h>

#include <Scheduler/Common/Clock.h>
#include <Scheduler/Lib/Task.h>
#include <iostream>

using std::chrono::seconds;
using namespace Scheduler;
using namespace Scheduler::Lib;

static Clock::time_point Future(const Clock::duration& length)
{
    return Clock::now() + length;
}

static Clock::time_point Past(const Clock::duration& length)
{
    return Clock::now() - length;
}

TEST(TaskConstruction, CreateSimpleTask)
{
    TaskPtr task = Task::Create();
    ASSERT_EQ(task->GetState(), TaskState::NEW);
    ASSERT_FALSE(task->IsExpired());
    ASSERT_FALSE(task->IsPremature());
    ASSERT_FALSE(task->HasDependencies());
}

TEST(TaskConstruction, CreateSimpleAfterTask)
{
    TaskPtr taskA = Task::After(Future(seconds(10)));
    ASSERT_EQ(taskA->GetState(), TaskState::NEW);
    ASSERT_TRUE(taskA->IsPremature());
    ASSERT_FALSE(taskA->IsExpired());

    TaskPtr taskB = Task::After(Past(seconds(10)));
    ASSERT_EQ(taskB->GetState(), TaskState::NEW);
    ASSERT_FALSE(taskB->IsPremature());
    ASSERT_FALSE(taskB->IsExpired());
}

TEST(TaskConstruction, CreateSimpleBeforeTask)
{
    TaskPtr taskA = Task::Before(Future(seconds(10)));
    ASSERT_EQ(taskA->GetState(), TaskState::NEW);
    ASSERT_FALSE(taskA->IsPremature());
    ASSERT_FALSE(taskA->IsExpired());

    TaskPtr taskB = Task::Before(Past(seconds(10)));
    ASSERT_EQ(taskB->GetState(), TaskState::NEW);
    ASSERT_FALSE(taskB->IsPremature());
    ASSERT_TRUE(taskB->IsExpired());
}

TEST(TaskConstruction, CreateSimpleBetweenTask)
{
    TaskPtr taskA = Task::Between(Clock::now(), Future(seconds(10)));
    ASSERT_EQ(taskA->GetState(), TaskState::NEW);
    ASSERT_FALSE(taskA->IsPremature());
    ASSERT_FALSE(taskA->IsExpired());

    TaskPtr taskB = Task::Between(Future(seconds(10)), Future(seconds(15)));
    ASSERT_EQ(taskB->GetState(), TaskState::NEW);
    ASSERT_TRUE(taskB->IsPremature());
    ASSERT_FALSE(taskB->IsExpired());

    TaskPtr taskC = Task::Between(Past(seconds(15)), Past(seconds(10)));
    ASSERT_EQ(taskC->GetState(), TaskState::NEW);
    ASSERT_FALSE(taskC->IsPremature());
    ASSERT_TRUE(taskC->IsExpired());
}

TEST(TaskDependencies, SimpleDependencies)
{
    TaskPtr taskA = Task::Create(),
            taskB = Task::Create(),
            taskC = Task::Create();

    ASSERT_FALSE(taskA->HasDependencies());
    ASSERT_FALSE(taskB->HasDependencies());
    ASSERT_FALSE(taskC->HasDependencies());

    taskA->Depends(taskB);
    ASSERT_TRUE(taskA->IsValid());
    ASSERT_TRUE(taskB->IsValid());
    ASSERT_TRUE(taskA->Requires(taskB));
    ASSERT_TRUE(taskA->HasDependencies());

    taskB->Depends(taskC);
    ASSERT_TRUE(taskC->IsValid());
    ASSERT_TRUE(taskC->IsValid());
    ASSERT_TRUE(taskB->Requires(taskC));
    ASSERT_TRUE(taskB->HasDependencies());

    ASSERT_TRUE(taskA->Requires(taskC));
}

TEST(TaskDependencies, CircularDependencies)
{
    TaskPtr taskA = Task::Create(),
            taskB = Task::Create(),
            taskC = Task::Create();

    ASSERT_FALSE(taskA->HasDependencies());
    ASSERT_FALSE(taskB->HasDependencies());
    ASSERT_FALSE(taskC->HasDependencies());

    taskA->Depends(taskB);
    taskB->Depends(taskC);
    taskC->Depends(taskA);

    ASSERT_FALSE(taskC->IsValid());
    ASSERT_FALSE(taskB->IsValid());
    ASSERT_FALSE(taskA->IsValid());
}