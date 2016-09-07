# Game of Life

This program shows the well known Game of Life. I've chosen this simulation as an exercise to explore:
* Member function pointers as argument
* Problem domain slicing
* Multi-threading, using all available cores



## (Member) function pointers as argument##
If you want to use function pointers as argument, where the function is a member function of a class, you also have to parse a pointer of the instance of the object. On the bottom of the page, there is a small code example showing the different cases for C style and C++ style function pointers.

The Game of Life simulation uses one function method for domain slicing and distributing the work load over the available CPU cores by starting an equal amount of threads. It takes a function pointer as an argument, to enable me to send the function for initial conditions and the function for time stepping to this method.

##Domain slicing##
The Game of Life simulation can be distributed in many different ways. Since it is a cellular automata, every cell is updated from t_0 -> t_1 fully independently. It only needs the states from its direct neighbours. This code used the domain slicing approach, which can easily be applied to other tasks, like image processing. The domain is divided horizontally in row batches. Each slice is then sent to another thread where its new state is computed. After the computation, the threads are joined and the new state is set as the current state (swap buffers). To prevent excessive memory allocation and destruction for each time step, only the memory range of the slice is given to the threads. All threads share the same memory block. This is fine, since they read the current state from buffer 1 and update only their own part in buffer 2.


**main.cpp**

```
#include <cmath>
#include "Foo.h"

int main() {
    Foo *foo = new Foo;

    foo->printOutputCFunction(sin, 1.57);
    foo->printOutputCppMemberFunction(&Foo::add, foo, 1, 2);
    foo->printOutputCppMemberFunctionSelf();
    return 0;
}
```

**Foo.h**
```
//
// Created by Gilbert François on 07-09-16.
//
#ifndef FUNCTIONPOINTERS_FOO_H
#define FUNCTIONPOINTERS_FOO_H

class Foo {

public:

    int add(int i, int j);

    // C style for calling function pointer
    void printOutputCFunction(double (*fn)(double), double x);

    // C++ style for calling member functions. It needs a second argument for the reference to the instance
    void printOutputCppMemberFunction(int (Foo::*fn)(int, int), Foo* foo, int i, int j);

    // C++ style for calling member functions, where the object instance calls one of its own functions and providing
    // the instance pointer by the reference 'this'.
    void printOutputCppMemberFunctionSelf();
};


#endif //FUNCTIONPOINTERS_FOO_H
```

**Foo.cpp**
```
//
// Created by Gilbert François on 07-09-16.
//
#include <iostream>
#include "Foo.h"

int Foo::add(int i, int j) {
    return i + j;
}

void Foo::printOutputCFunction(double (*fn)(double), double x) {
    double y = fn(x);
    std::cout << std::to_string(y) << std::endl;
}

void Foo::printOutputCppMemberFunction(int (Foo::*fn)(int, int), Foo* foo, int i, int j) {
    int k = (foo->*fn)(i, j);
    std::cout << k << std::endl;
}

void Foo::printOutputCppMemberFunctionSelf() {
    printOutputCppMemberFunction(&Foo::add, this, 7, 3);
}
```