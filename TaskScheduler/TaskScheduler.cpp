#include "stdafx.h"

#include <thread>
#include <chrono>
#include "TaskScheduler.h"

using namespace lb;

TaskScheduler& TaskScheduler::GetInstance() {
	static TaskScheduler instance;
	return instance;
}

TaskScheduler::TaskScheduler() : m_running(false), m_ticks(0) {
}

void TaskScheduler::Loop() {
	auto start = std::chrono::high_resolution_clock::now();
	m_running = true;
	while (m_running) {
		start = std::chrono::high_resolution_clock::now();
		lock.lock();
		for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
			std::shared_ptr<Task>& task = it->second;
			if (task->m_enabled && !task->m_startHandlerRun) {
				bool allowed = CheckTask(task);
				if (allowed) {
					for (auto it = task->m_requiredResources.begin(); it != task->m_requiredResources.end(); ++it) {
						m_usedResources.insert(*it);
					}
					task->OnEnable();
					task->m_startHandlerRun = true;
				} else {
					task->m_enabled = false;
					task->m_startHandlerRun = true;
					task->m_endHandlerRun = true;
				}
			}
			if (task->m_enabled && m_ticks % task->m_tickInterval == 0) {
				task->Run();
			}
			if (!task->m_enabled && !task->m_endHandlerRun) {
				task->OnDisable();
				task->m_endHandlerRun = true;
				for (auto it = task->m_requiredResources.begin(); it != task->m_requiredResources.end(); ++it) {
					m_usedResources.erase(*it);
				}
			}
		}

		for (auto it = m_removeTasks.begin(); it != m_removeTasks.end(); it++) {
			m_tasks.erase((*it)->m_id);
		}
		m_removeTasks.erase(m_removeTasks.begin(), m_removeTasks.end());
		lock.unlock();
		m_ticks++;
		std::this_thread::sleep_until(start + std::chrono::milliseconds(TICK_SPEED));
	}
}

void TaskScheduler::AddTask(std::shared_ptr<Task> task) {
	std::lock_guard<std::mutex> guard(lock);
	m_tasks.insert(std::make_pair(task->m_id, task));
}

void TaskScheduler::RemoveTask(std::shared_ptr<Task> task) {
	std::lock_guard<std::mutex> guard(lock);
	task->SetEnabled(false);
	m_removeTasks.push_back(task);
}

void TaskScheduler::Stop() {
	m_running = false;
}

int TaskScheduler::GetTicks() {
	return m_ticks;
}

bool lb::TaskScheduler::CheckTask(std::shared_ptr<Task>& task) {
	for (auto it = task->m_requiredResources.begin(); it != task->m_requiredResources.end(); ++it) {
		if (m_usedResources.find(*it) != m_usedResources.end()) {
			return false;
		}
	}
	return true;
}

int TaskScheduler::GenerateId() {
	static int nextId;
	return nextId++;
}

TaskScheduler::~TaskScheduler() {
}

int main() {
	return 0;
}