/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

#include "Delegate.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <list>
#include <functional>

namespace novus
{

template <typename return_type, typename ... params>
class MulticastDelegate
{
public:
	void operator() (params... xs) const
	{
		for (const auto& d : Delegates)
		{
			d(xs...);
		}
	}

	MulticastDelegate& operator+= (const Delegate<return_type, params...>& d)
	{
		Delegates.push_back(d);

		return *this;
	}

	MulticastDelegate& operator-= (const Delegate<return_type, params...>& d)
	{
		auto endIt = std::remove_if(Delegates.begin(), Delegates.end(),
			[&](const Delegate<return_type, params...>& other)
		{ return d == other; });

		Delegates.erase(endIt, Delegates.end());

		return *this;
	}

	/**
	 *	Removes all Delegates from this MulticastDelegate
	 */
	void Clear()
	{
		Delegates.clear();
	}

	/**
	 *	Get number of target delegates
	 */
	size_t GetSize() const
	{
		return Delegates.size();
	}

private:
	std::list<Delegate<return_type, params...>> Delegates;
};

};