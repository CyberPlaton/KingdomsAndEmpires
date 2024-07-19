#include "imnodes_internal.h"
#include <core.h>

//------------------------------------------------------------------------------------------------------------------------
cscoped_color_variable::cscoped_color_variable(ImNodesCol var, unsigned abgr)
{
	push(var, abgr);
}

//------------------------------------------------------------------------------------------------------------------------
cscoped_color_variable::~cscoped_color_variable()
{
	reset();
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_color_variable::push(ImNodesCol var, unsigned abgr)
{
	ImNodes::PushColorStyle(var, abgr);
	++m_count;
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_color_variable::pop()
{
	CORE_ASSERT(m_count > 0, "Invalid operation. Mismatched 'push' and 'pop' call count!");

	ImNodes::PopColorStyle();
	--m_count;
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_color_variable::reset()
{
	for (auto i = 0u; i < m_count; ++i)
	{
		pop();
	}
}

//------------------------------------------------------------------------------------------------------------------------
cscoped_style_variable::cscoped_style_variable(ImNodesStyleVar var, const ImVec2& value)
{
	push(var, value);
}

//------------------------------------------------------------------------------------------------------------------------
cscoped_style_variable::cscoped_style_variable(ImNodesStyleVar var, float value)
{
	push(var, value);
}

//------------------------------------------------------------------------------------------------------------------------
cscoped_style_variable::~cscoped_style_variable()
{
	reset();
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_style_variable::push(ImNodesStyleVar var, const ImVec2& value)
{
	ImNodes::PushStyleVar(var, value);
	++m_count;
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_style_variable::push(ImNodesStyleVar var, float value)
{
	ImNodes::PushStyleVar(var, value);
	++m_count;
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_style_variable::pop()
{
	CORE_ASSERT(m_count > 0, "Invalid operation. Mismatched 'push' and 'pop' call count!");

	ImNodes::PopStyleVar();
	--m_count;
}

//------------------------------------------------------------------------------------------------------------------------
void cscoped_style_variable::reset()
{
	for (auto i = 0u; i < m_count; ++i)
	{
		pop();
	}
}

//------------------------------------------------------------------------------------------------------------------------
IImNode::~IImNode()
{
}

//------------------------------------------------------------------------------------------------------------------------
ImNodeData& IImNode::internal_data(int id)
{
	auto& context = ImNodes::EditorContextGet();
	const int idx = ImNodes::ObjectPoolFindOrCreateIndex(context.Nodes, id);
	return context.Nodes.Pool[idx];
}

//------------------------------------------------------------------------------------------------------------------------
ImNodeAllocator::~ImNodeAllocator()
{
	while(!m_allocations.empty())
	{
		deallocate(m_allocations.begin()->second);
	}
}

//------------------------------------------------------------------------------------------------------------------------
void* ImNodeAllocator::allocate(uint64_t n)
{
#if DLMALLOC_ENABLE
	void* p = CORE_MALLOC(n);
#else
	void* p = ImGui::MemAlloc(n);
#endif
	m_allocations[(uint64_t)p] = p;

	return p;
}

//------------------------------------------------------------------------------------------------------------------------
void ImNodeAllocator::deallocate(void* p)
{
#if DLMALLOC_ENABLE
	CORE_FREE(p);
#else
	ImGui::MemFree(p);
#endif
	m_allocations.erase((uint64_t)p);
}
