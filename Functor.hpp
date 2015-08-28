#include <tuple>
#include <utility>
#include "Functor.h"

template <typename Ret, typename Tuple>
class FunctorImpl
{
public:
	virtual Ret operator() (Tuple&& args) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {}
};


template <typename Result, typename ... Args>
Functor<Result, Args...>::Functor () : spImpl_(nullptr) {}

template <typename Result, typename ... Args>
Functor<Result, Args...>::Functor (Functor&& rhs)
	: spImpl_(std::move(rhs)) {}

template <typename Result, typename ... Args>
Functor<Result, Args...>::Functor(std::unique_ptr<Impl> spImpl)
	: spImpl_(std::move(spImpl)) {}

template <typename Result, typename ... Args>
template <typename Func>
Functor<Result, Args...>::Functor (Func&& func)
	: spImpl_(new FunctorHandler<Functor<Result, Args...>, Func>(std::forward<Func>(func)))
{}

template <typename Result, typename ... Args>
template <class PtrObj, typename PtrMemFn>
Functor<Result, Args...>::Functor (PtrObj&& pObj, PtrMemFn pMemFn)
	: spImpl_(new MemFuncHandler<Functor<Result, Args...>, PtrObj, PtrMemFn>(std::forward<PtrObj>(pObj), pMemFn))
{}


template <typename Result, typename ... Args>
Functor<Result, Args...>& Functor<Result, Args...>::operator= (const Functor& rhs)
{
	Functor copy(rhs);

	Impl* p = spImpl_.release();
	spImpl_.reset(copy.spImpl_.release());
	copy.spImpl_.reset(p);

	return *this;
}

template <typename Result, typename ... Args>
Result Functor<Result, Args...>::operator() (Args&&... args)
{
	return (*spImpl_)(std::forward_as_tuple(args...));
}

template <typename ParentFunctor, typename Func>
class FunctorHandler 
: public FunctorImpl<typename ParentFunctor::ResultType, typename ParentFunctor::Params>
{
public:
	using ResultType = typename ParentFunctor::ResultType;
	using Params = typename ParentFunctor::Params;

private:
	template<typename Func, typename Tup, std::size_t... index>
	decltype(auto) InvokeHelper (Func&& func, Tup&& tup, std::index_sequence<index...>)
	{
		return func(std::get<index>(std::forward<Tup>(tup))...);
	}

	template<typename Func, typename Tup>
	decltype(auto) Invoke (Func&& func, Tup&& tup)
	{
		constexpr auto Size = std::tuple_size<typename std::decay<Tup>::type>::value;
		return InvokeHelper(std::forward<Func>(func),
			std::forward<Tup>(tup),
			std::make_index_sequence<Size>{});
	}

public:
	FunctorHandler (Func&& func) : func_(std::forward<Func>(func)) {}

	FunctorHandler* Clone() const override
	{ return new FunctorHandler(*this); }

	ResultType operator() (Params&& args) override
	{
		return Invoke(std::forward<Func>(func_), std::forward<Params>(args));
	}

private:
	Func func_;
};


template <typename ParentFunctor, typename PointerToObj, typename PointerToMemFn>
class MemFuncHandler 
: public FunctorImpl<typename ParentFunctor::ResultType, typename ParentFunctor::Params>
{
public:
	using ResultType = typename ParentFunctor::ResultType;
	using Params = typename ParentFunctor::Params;

private:
	template <typename PtrObj, typename PtrMemFn, typename Tup, std::size_t... index>
	decltype(auto) InvokeHelper (PtrObj&& pObj, PtrMemFn pMemFn, Tup&& tup, std::index_sequence<index...>)
	{
		return ((*pObj).*pMemFn)(std::get<index>(std::forward<Tup>(tup))...);
	}

	template<typename PtrObj, typename PtrMemFn, typename Tup>
	decltype(auto) Invoke(PtrObj&& pObj, PtrMemFn pMemFn, Tup&& tup)
	{
		constexpr auto Size = std::tuple_size<typename std::decay<Tup>::type>::value;
		return InvokeHelper(std::forward<PtrObj>(pObj),
			pMemFn,
			std::forward<Tup>(tup),
			std::make_index_sequence<Size>{});
	}

public:
	MemFuncHandler (PointerToObj&& pObj, PointerToMemFn pMemFn)
		: pObj_(std::forward<PointerToObj>(pObj)), pMemFn_(pMemFn)
	{}

	MemFuncHandler* Clone() const override
	{ return new MemFuncHandler(*this); }

	ResultType operator() (Params&& args) override
	{ 
		return Invoke(std::forward<PointerToObj>(pObj_), pMemFn_, std::forward<Params>(args));
	}

private:
	PointerToObj pObj_;
	PointerToMemFn pMemFn_;
};


/*
template <typename IncomingFunc, typename ResultType, typename First, typename ... Rest>
class BinderFirst : public FunctorImpl<ResultType, Rest...>
{
public:
	BinderFirst (IncomingFunc&& func, First first)
		: func_(std::forward<IncomingFunc>(func)), bound_(first)
	{}
	
	BinderFirst* Clone() const override
	{ return new BinderFirst(*this); }

	ResultType operator()(Rest... args)
	{
		return func_(bound_, std::forward<Rest>(args)...);
	}
private:
	IncomingFunc func_;
	First bound_;
};
*/
