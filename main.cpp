#include <iostream>
#include <string>
#include <functional>
#include "Functor.h"
#include "Functor.hpp"

using namespace std;

class Test
{
public:
	void Foo(int a) 
	{
		cout << "Foo of Test: " << a << endl;
	}
};

void TestFunction(int i, double d)
{
	cout << "TestFunction (" << i << ", " << d << ") called" << endl;
}


int main(int argc, char* argv[])
{
	// 1. Normal Function
	Functor<void, int, double> fn(TestFunction);
	
	// 2. Member Function
	Test a;
	Functor<void, int> mf(&a, &Test::Foo);

	// 3. Lambda
	Functor<void, int> lambda([](int a) {
		++a;
		cout << a << endl;
	});
	
	fn(4, 3.5);
	mf(5);
	lambda(3);
	

	return 0;
}