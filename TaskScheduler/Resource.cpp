#include "stdafx.h"
#include "Resource.h"

using namespace lb;

Resource::Resource() {
	m_id = TaskScheduler::GenerateId();
}

Resource::~Resource() {
}
