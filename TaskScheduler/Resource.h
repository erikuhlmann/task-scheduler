#pragma once

#include "TaskScheduler.h"
#include "Task.h"

namespace lb {
	class Resource {
		friend class TaskScheduler;
		friend class Task;
	protected:
		int m_id;
	public:
		Resource();
		virtual ~Resource();
	};
}