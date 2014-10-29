#include "currying.h"

#include <iostream>

int add(int a, int b) {
	return a + b;
}

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

	auto ma_add = curry_multiapply(add);

	std::cout << "Multi-application: ";
	std::cout << ma_add(1)(2) << " == " << ma_add(1, 2) << std::endl;

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}
