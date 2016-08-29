#pragma once

#include "TaskScheduler.h"
#include "Resource.h"

namespace lb {
	class Resource;

	class Task {
		friend class TaskScheduler;

	protected:
		bool m_enabled;
		bool m_startHandlerRun;
		bool m_endHandlerRun;
		int m_tickInterval;
		int m_id;

		std::unordered_set<int> m_requiredResources;

		void SetInterval(int tickInterval);
	public:
		Task();
		Task(int tickInterval);
		virtual ~Task();

		virtual void OnEnable() = 0;
		virtual void Run() = 0;
		virtual void OnDisable() = 0;

		void Requires(std::shared_ptr<Resource> resource);

		bool IsEnabled();
		void SetEnabled(bool enabled);
		int GetTickInterval();
	};
}