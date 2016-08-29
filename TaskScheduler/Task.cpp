#include "stdafx.h"
#include "Task.h"
#include "TaskScheduler.h"
#include "Resource.h"

using namespace lb;

Task::Task() : Task(1) {
}

Task::Task(int tickInterval) : m_tickInterval(tickInterval) {
	m_enabled = false;
	m_startHandlerRun = false;
	m_endHandlerRun = false;
	m_id = TaskScheduler::GenerateId();
}

void Task::Requires(std::shared_ptr<Resource> resource) {
	m_requiredResources.insert(resource->m_id);
}

void Task::SetInterval(int tickInterval) {
	m_tickInterval = tickInterval;
}

bool Task::IsEnabled() {
	return m_enabled;
}

void Task::SetEnabled(bool enabled) {
	if (enabled && !m_enabled) {
		m_startHandlerRun = false;
	} else if(!enabled && m_enabled) {
		m_endHandlerRun = false;
	}

	m_enabled = enabled;
}

int Task::GetTickInterval() {
	return m_tickInterval;
}

Task::~Task() {
}
