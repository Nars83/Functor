#pragma once

#include <memory>
#include <tuple>

template <typename R, typename ... Args>
class FunctorImpl;

template <typename Result, typename ... Args>
class Functor
{
public:
	using Impl = FunctorImpl<Result, Args...>;

public:
	Functor ();
	Functor (Functor&& rhs);
	explicit Functor (std::unique_ptr<Impl> spImpl);

	template <typename Func>
	Functor (Func&& func);

	template <class PtrObj, typename PtrMemFn>
	Functor (PtrObj&& pObj, PtrMemFn pMemFn);

	Functor& operator= (const Functor& rhs);
	
	Result operator() (Args&&... args);
private:
	std::unique_ptr<Impl> spImpl_;
};
