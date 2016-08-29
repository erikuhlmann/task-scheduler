#include "stdafx.h"
#include "CppUnitTest.h"

#include <thread>
#include <chrono>

// HACKS
#define protected public
#include <TaskScheduler.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace lb;

class TestResource: public Resource {
public:
	TestResource() {}
	virtual ~TestResource() {}
};

std::shared_ptr<Resource> res1(new TestResource());

class TestTask : public Task {
public:
	int m_enableCount = 0;
	int m_disableCount = 0;
	int m_runCount = 0;
	TestTask() {
		Requires(res1);
	}
	virtual ~TestTask() {}

	void OnEnable() {
		m_enableCount++;
	}
	void OnDisable() {
		m_disableCount++;
	}
	void Run() {
		m_runCount++;
	}
};

DWORD WINAPI SchedulerLoop(__in LPVOID lpParameter) {
	TaskScheduler::GetInstance().Loop();
	return 0;
}

namespace UnitTest {
	DWORD threadID1;
	HANDLE h1;
	TEST_CLASS(UnitTest1) {
	public:
		TEST_CLASS_INITIALIZE(UnitTest1Init) {
			h1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SchedulerLoop, 0, 0, &threadID1);
		}

		TEST_METHOD(AddRemoveTest) {
			std::shared_ptr<Task> t1(new TestTask());
			std::shared_ptr<Task> t2(new TestTask());
			TaskScheduler::GetInstance().AddTask(t1);
			TaskScheduler::GetInstance().AddTask(t2);
			t1->SetEnabled(true);
			t2->SetEnabled(true);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			TaskScheduler::GetInstance().RemoveTask(t1);
			TaskScheduler::GetInstance().RemoveTask(t2);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			Assert::IsTrue(TaskScheduler::GetInstance().m_tasks.size() == 0);
		}

		TEST_METHOD(ResourceLockTest) {
			std::shared_ptr<Task> t1(new TestTask());
			std::shared_ptr<Task> t2(new TestTask());
			TaskScheduler::GetInstance().AddTask(t1);
			TaskScheduler::GetInstance().AddTask(t2);
			t1->SetEnabled(true);
			t2->SetEnabled(true);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			Assert::IsTrue(t1->IsEnabled() ^ t2->IsEnabled());
		}

		TEST_METHOD(EnableDisableTest) {
			std::shared_ptr<TestTask> t1(new TestTask());
			TaskScheduler::GetInstance().AddTask(t1);
			t1->SetEnabled(true);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			Assert::IsTrue(t1->m_enableCount == 1);
			t1->SetEnabled(false);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			Assert::IsTrue(t1->m_disableCount == 1);
			Assert::IsTrue(t1->m_runCount > 0);
		}

		// add some more test here

		TEST_METHOD_CLEANUP(UnitTest1Reset) {
			std::lock_guard<std::mutex> guard(TaskScheduler::GetInstance().lock);
			TaskScheduler::GetInstance().m_tasks.clear();
			TaskScheduler::GetInstance().m_removeTasks.clear();
			TaskScheduler::GetInstance().m_usedResources.clear();
			TaskScheduler::GetInstance().m_ticks = 0;
		}

		TEST_CLASS_CLEANUP(UnitTest1Cleanup) {
			TaskScheduler::GetInstance().Stop();
			WaitForSingleObject(h1, INFINITE);
		}

	};
}