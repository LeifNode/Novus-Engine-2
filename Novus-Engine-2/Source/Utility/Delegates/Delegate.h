//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

/**
 *	Variadic template based delegate system
 *	Based on method from http://blog.coldflake.com/posts/C++-delegates-on-steroids/ and 
 *		http://www.codeproject.com/Articles/136799/Lightweight-Generic-C-Callbacks-or-Yet-Another-Del
 *
 *	Probably going to avoid using this since std::function works with a lot of other STL stuff already that I don't have the need to remake at the moment.
 *	The main difference is that Delegate objects are comparible for equality unlike std::function since they don't allow as much flexibility and don't have to worry about non comparible functions
 *	This implementation does allow you to construct novus::Delegate objects from functions about 10x faster than std::function objects, but that probably won't matter in most situations
 */

/**
 *	Features:
 *			- Supports functions with any number of arguments (though it's probably a good idea to keep that number under 10)
 *			- Supports returning one value of any type from the function call
 *			- Supports functions marked as const
 */

/**
 *	TODO:
 *		 - Lambda functor support
 *		 - Static/global function support
 */

/**
 *	Usage: Call NE_Delegate to create a delegate
 *	Pass a pointer to the object that the delegate will operate on along with the function pointer in the object that the delegate will invoke
 *	Example: struct Foo { void Bar(); }
 *			 Foo foo();
 *			 auto d = NE_Delegate(&foo, &Foo::Bar);
 *			 d(); //Invoke the Bar function on foo
 */

namespace novus
{

template <typename return_type, typename ... params>
class Delegate
{
	typedef return_type(*Type)(void* callee, params ...);
public:
	Delegate()
		:Callee(nullptr),
		CallbackFunction(nullptr)
	{}

	Delegate(void* callee, Type function)
		:Callee(callee),
		CallbackFunction(function)
	{}

	template <class T, return_type (T::*TFunction)(params...)>
	static Delegate FromFunction(T* callee)
	{
		Delegate d(callee, &FunctionCaller<T, TFunction>);
		return d;
	}

	/**
	 *	Simplifies delegate calls
	 */
	return_type operator()(params ... xs) const
	{
		return (*CallbackFunction)(Callee, xs...);
	}

	bool operator== (const Delegate<return_type, params...>& other) const
	{
		return this->Callee == other.Callee && this->CallbackFunction == other.CallbackFunction;
	}

	bool operator!= (const Delegate<return_type, params...>& other) const
	{
		return this->Callee != other.Callee || this->CallbackFunction != other.CallbackFunction;
	}

	/**
	 *	Comparisons against invalid alternate types of delegates
	 */
	template <typename return_type2, typename ... params2>
	bool operator==(const Delegate<return_type2, params2...>& other)
	{
		return false;
	}

	template <typename return_type2, typename ... params2>
	bool operator!=(const Delegate<return_type2, params2...>& other)
	{
		return true;
	}

	bool IsValid() const
	{
		return CallbackFunction != nullptr;
	}

private:
	void* Callee;
	Type CallbackFunction;

	template <class T, return_type(T::*TFunction)(params...)>
	static return_type FunctionCaller(void* callee, params... xs)
	{
		assert(callbackFunction != nullptr);

		T* p = static_cast<T*>(callee);
		return (p->*TFunction)(xs...);
	}
};

namespace detail
{
	template <typename T, typename return_type, typename... params>
	struct DelegateFactory
	{
		template <return_type(T::*Func)(params...)>
		static return_type FunctionCaller(void* o, params... xs)
		{
			return (static_cast<T*>(o)->*Func)(xs...);
		}

		template <return_type(T::*Func)(params...) const>
		static return_type FunctionCallerConst(void* o, params... xs)
		{
			return (static_cast<T*>(o)->*Func)(xs...);
		}

		template <return_type(T::*Func)(params...)>
		static Delegate<return_type, params...> Bind(T* o)
		{
			return Delegate<return_type, params...>(o, &DelegateFactory::FunctionCaller<Func>);
		}

		template <return_type(T::*Func)(params...) const>
		static Delegate<return_type, params...> Bind(T* o)
		{
			return Delegate<return_type, params...>(o, &DelegateFactory::FunctionCallerConst<Func>);
		}
	};

	template <typename T, typename return_type, typename... params>
	DelegateFactory<T, return_type, params...> MakeDelegateFactory(return_type(T::*)(params...))
	{
		return DelegateFactory<T, return_type, params...>();
	}

	template <typename T, typename return_type, typename... params>
	DelegateFactory<T, return_type, params...> MakeDelegateFactory(return_type(T::*)(params...) const)
	{
		return DelegateFactory<T, return_type, params...>();
	}
}

//template <typename T, typename return_type, typename ... params>
//Delegate<return_type, params...> MakeDelegateNE(T* o, return_type(T::*func)(params...))
//{
//	return detail::MakeDelegateFactory(func).Bind<>(o);
//}

//Not sure how to put this is a function at the moment
#define NE_Delegate(ptr, func) (detail::MakeDelegateFactory(func).Bind<func>(ptr))

};