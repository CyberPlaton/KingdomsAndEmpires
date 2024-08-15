#include "imnodes_scopes.hpp"

namespace imnodes
{
	//- Scoped node.
	//------------------------------------------------------------------------------------------------------------------------
	class cnode final
	{
	public:
		static constexpr auto C_INVALID_ID = 0;

		cnode(const ImNodes::NodeId& id);
		~cnode();

	private:
		ImNodes::NodeId m_id;
	};

} //- imnodes