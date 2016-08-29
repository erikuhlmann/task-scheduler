#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <thread>
#include "Task.h"
#include "Resource.h"

namespace lb {

	class Task;
	class Resource;

	class TaskScheduler {
	protected:
		std::mutex lock;
		bool m_running;
		int m_ticks;

		std::unordered_map<int, std::shared_ptr<Task>> m_tasks;
		std::unordered_set<int> m_usedResources;
		std::vector<std::shared_ptr<Task>> m_removeTasks;

		bool CheckTask(std::shared_ptr<Task>& task);
	public:
		static constexpr int TICK_SPEED = 1000 / 50; // ms

		TaskScheduler();
		virtual ~TaskScheduler();
		void Loop();
		void AddTask(std::shared_ptr<Task> task);
		void RemoveTask(std::shared_ptr<Task> task);
		void Stop();
		int GetTicks();

		static int GenerateId();

		static TaskScheduler& GetInstance();
	};

}