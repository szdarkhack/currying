#pragma once

template <typename F, typename... Args>
struct currier;
 
template <typename F, typename Last>
struct currier<F, Last>
{
	static auto curry1(F func) { return func; }
};
 
template <typename F, typename Head, typename... Tail>
struct currier<F, Head, Tail...>
{
	static auto curry1(F func) {
		return [=](Head head) mutable {
			auto lambda = [=](Tail... tail) mutable {
				return func(head, tail...);
			};
			return currier<decltype(lambda), Tail...>::curry1(lambda);
		};
	}
};
 
// -------- WRAPPERS --------
 
template <typename F>
struct curry_t;
 
template <typename R, typename... Args>
struct curry_t<R(Args...)>
{
	using type = currier<R(Args...), Args...>;
};
 
template <typename R, typename... Args>
struct curry_t<R(*)(Args...)>
{
	using type = currier<R(*)(Args...), Args...>;
};
 
template <typename T>
struct curry_t : public curry_t<decltype(&T::operator())> {};
 
template <typename Owner, typename R, typename... Args>
struct curry_t<R(Owner::*)(Args...) const>
{
	using type = currier<Owner, Args...>;
};
 
template <typename Owner, typename R, typename... Args>
struct curry_t<R(Owner::*)(Args...)>
{
	using type = currier<Owner, Args...>;
};
 
// maybe add volatile and ref qualifiers here?
 
// -------- MultiApplier --------
 
#include <utility>
 
template <typename C>
struct applier {
	applier(C&& func) : _func(std::move(func)) {}
 
	template <typename Head, typename... Tail>
	auto operator()(Head&& head, Tail&&... tail) {
		auto ret = _func(std::forward<Head>(head));
		return applier<decltype(ret)>(std::move(ret))(std::forward<Tail>(tail)...);
	}
 
	template <typename Head>
	auto operator()(Head&& head) {
		return _func(std::forward<Head>(head));
	}
 
private:
	C _func;
};
 
// -------- Entry Point --------
 
template <typename F>
auto curry(F func) {
	return curry_t<F>::type::curry1(func);
}
 
template <typename F>
auto curry_multiapply(F func) {
	auto ret = curry_t<F>::type::curry1(func);
	return applier<decltype(ret)>(std::move(ret));
}
