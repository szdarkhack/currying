#pragma once

#include <utility>
#include <functional>

namespace currying {

	template <typename F, typename... Args>
	struct currier;

	template <typename F, typename Last>
	struct currier < F, Last >
	{
		static constexpr int nargs = 1;
		static auto curry1(F func) { return func; }
	};

	template <typename F, typename Head, typename... Tail>
	struct currier<F, Head, Tail...>
	{
		static constexpr int nargs = 1 + sizeof...(Tail);
		static auto curry1(F func) {
			return [=](Head head) mutable {
				auto lambda = [=, head = std::move(head)](Tail... tail) mutable {
					return func(std::move(head), tail...);
				};
				return currier<decltype(lambda), Tail...>::curry1(std::move(lambda));
			};
		}
	};

	template <typename F, typename Head, typename... Tail>
	struct currier<F, Head&, Tail...>
	{
		static constexpr int nargs = 1 + sizeof...(Tail);
		static auto curry1(F func) {
			return [=](Head& head) mutable {
				auto lambda = [=, head = std::ref(head)](Tail... tail) mutable {
					return func(head, tail...);
				};
				return currier<decltype(lambda), Tail...>::curry1(std::move(lambda));
			};
		}
	};

	template <typename F, typename Head, typename... Tail>
	struct currier<F, Head&&, Tail...>
	{
		static constexpr int nargs = 1 + sizeof...(Tail);
		static auto curry1(F func) {
			return [=](Head&& head) mutable {
				auto lambda = [=, head = std::move(head)](Tail... tail) mutable {
					return func(std::move(head), tail...);
				};
				return currier<decltype(lambda), Tail...>::curry1(std::move(lambda));
			};
		}
	};

	// -------- WRAPPERS --------

	template <typename F>
	struct curry_t;

	template <typename R, typename... Args>
	struct curry_t < R(Args...) >
	{
		using type = currier < R(Args...), Args... > ;
	};

	template <typename R, typename... Args>
	struct curry_t < R(*)(Args...) >
	{
		using type = currier < R(*)(Args...), Args... > ;
	};

	template <typename T>
	struct curry_t : public curry_t < decltype(&T::operator()) > {};

	template <typename Owner, typename R, typename... Args>
	struct curry_t < R(Owner::*)(Args...) const >
	{
		using type = currier < Owner, Args... > ;
	};

	template <typename Owner, typename R, typename... Args>
	struct curry_t < R(Owner::*)(Args...) >
	{
		using type = currier < Owner, Args... > ;
	};

	// maybe add volatile and ref qualifiers here?

	// -------- MultiApplier --------

	template <typename C, int NARGS>
	struct applier {
		applier(C&& func) : _func(std::move(func)) {}

		template <typename Head, typename... Tail>
		auto operator()(Head&& head, Tail&&... tail) {
			static_assert(NARGS > 0, "Too many arguments");
			auto ret = _func(std::forward<Head>(head));
			return applier<decltype(ret), NARGS - 1>(std::move(ret))(std::forward<Tail>(tail)...);
		}

		template <typename Single>
		auto operator()(Single&& arg) {
			auto ret = _func(std::forward<Single>(arg));
			return applier<decltype(ret), NARGS - 1>(std::move(ret));
		}

	private:
		C _func;
	};

	template <typename C>
	struct applier < C, 1 > {
		applier(C&& func) : _func(std::move(func)) {}

		template <typename Arg>
		auto operator()(Arg&& arg) {
			return _func(std::forward<Arg>(arg));
		}

	private:
		C _func;
	};

}

// -------- Entry Point --------
 
template <typename F>
auto curry(F func) {
	return currying::curry_t<F>::type::curry1(func);
}
 
template <typename F>
auto curry_multiapply(F func) {
	auto ret = currying::curry_t<F>::type::curry1(func);
	return currying::applier<decltype(ret), currying::curry_t<F>::type::nargs>(std::move(ret));
}
