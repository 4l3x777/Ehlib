#pragma once

#include <string>
#include <map>

#include "mongoose/mongoose.h"
#include "params.h"


namespace ehlib
{
	namespace mongoose_engine_adapter
	{
		void request();
	}

	// ehlib main interfase
	template <typename ...Args>
	Response request(Args&& ...args)
	{
		// reset response struct
		thread_structures::fromSource.reset();

		// handle input params & prepare them for mongoose engine requset
		std::initializer_list<int> initial = { (params_handlers::Params(args), 0)... };

		// mongoose engine request
		mongoose_engine_adapter::request();

		// reset request struct
		thread_structures::toSource.reset();

		// return copy of response struct
		return thread_structures::fromSource;
	};

}