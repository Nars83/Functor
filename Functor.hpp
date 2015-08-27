#include "Functor.h"

template <typename R, typename ... Args>
class FunctorImpl
{
public:
	virtual R operator() (Args&&... args) = 0;
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
	: spImpl_(new FunctorHandler<Func, Result, Args...>(std::forward<Func>(func)))
{}

template <typename Result, typename ... Args>
template <class PtrObj, typename PtrMemFn>
Functor<Result, Args...>::Functor (PtrObj&& pObj, PtrMemFn pMemFn)
	: spImpl_(new MemFuncHandler<PtrObj, PtrMemFn, Result, Args...>(std::forward<PtrObj>(pObj), pMemFn))
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
	return (*spImpl_)(std::forward<Args>(args)...);
}

template <typename Func, typename ResultType, typename ... Args>
class FunctorHandler : public FunctorImpl<ResultType, Args...>
{
public:
	FunctorHandler(Func&& func) : func_(std::forward<Func>(func)) {}
	FunctorHandler* Clone() const override
	{
		return new FunctorHandler(*this);
	}

	ResultType operator() (Args&&... args) override
	{
		return func_(std::forward<Args>(args)...);
	}

private:
	Func func_;
};


template <typename PointerToObj, typename PointerToMemFn, typename ResultType, typename ... Args>
class MemFuncHandler : public FunctorImpl<ResultType, Args...>
{
public:
	MemFuncHandler (PointerToObj&& pObj, PointerToMemFn pMemFn)
		: pObj_(std::forward<PointerToObj>(pObj)), pMemFn_(pMemFn)
	{}

	MemFuncHandler* Clone() const override
	{
		return new MemFuncHandler(*this);
	}

	ResultType operator() (Args&&... args) override
	{
		return ((*pObj_).*pMemFn_)(std::forward<Args>(args)...);
	}

private:
	PointerToObj pObj_;
	PointerToMemFn pMemFn_;
};