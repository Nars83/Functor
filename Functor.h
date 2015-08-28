#pragma once

#include <memory>
#include <tuple>

template <typename Ret, typename Tuple>
class FunctorImpl;

template <typename Ret, typename ... Args>
class Functor
{
public:
	using Params = std::tuple<Args...>;
	using ResultType = Ret;
	using Impl = FunctorImpl<Ret, std::tuple<Args...>>;

public:
	Functor ();
	Functor (Functor&& rhs);
	explicit Functor (std::unique_ptr<Impl> spImpl);

	template <typename Func>
	Functor (Func&& func);

	template <class PtrObj, typename PtrMemFn>
	Functor (PtrObj&& pObj, PtrMemFn pMemFn);

	Functor& operator= (const Functor& rhs);
	// Functor& operator= (Functor&& rhs);

	Ret operator() (Args&&... args);
private:
	std::unique_ptr<Impl> spImpl_;
};
