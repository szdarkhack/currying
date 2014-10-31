#include "currying.h"

#include <iostream>

int add(int a, int b) {
	return a + b;
}

struct Test
{
	Test() = default;
	Test(const Test&) {
		std::cout << "Copy constructor\n";
	}
	Test(Test&&) {
		std::cout << "Move constructor\n";
	}
};

int main() {
	auto c_add = curry(add);
	auto inc = c_add(1);

	std::cout << "Normal function: ";
	std::cout << inc(5) << std::endl;

	auto lambda = [](int a, int b) { return a + b; };
	auto c_lambda = curry(lambda);
	auto l_inc = c_lambda(1);

	std::cout << "Lambda: ";
	std::cout << l_inc(5) << std::endl;

	auto mlambda = [](int a, int b) mutable { return a + b; };
	auto c_mlambda = curry(mlambda);
	auto ml_inc = c_mlambda(1);

	std::cout << "Mutable lambda: ";
	std::cout << ml_inc(5) << std::endl;

	auto ma_add = curry_multiapply([](int a, int b, int c) {
		return a + b + c;
	});

	std::cout << "Multi-application: ";
	std::cout 
		<< ma_add(1,1,1) << " == "
		<< ma_add(1,1)(1) << " == "
		<< ma_add(1)(1)(1) << " == " 
		<< ma_add(1)(1,1) << std::endl;

	auto refc = curry_multiapply([](int& x, int a) {
		x += a;
	});

	int x = 4;

	std::cout << "Reference parameter: ";
	refc(x)(1);
	refc(x, 1);
	std::cout << x << std::endl;

	Test t;
	std::cout << "Passing by value:\n";
	byvalue(t)(0);
	std::cout << "Passing by reference:\n";
	byref(t)(0);
	std::cout << "Passing by move:\n";
	byvalue(std::move(t))(0);

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}
