# Game of Life

This program shows the well known Game of Life. I've chosen this simulation as an exercise to explore:
- Problem domain slicing
- Multi-threading
- Member function pointers as argument

If you want to use function pointers as argument, where the function is a member of a class, you also have to parse a pointer
of the instance.

In C:
```
void doSomething(void (*fn)(int, int)) {
    fn(3, 5);
}

```

In C++
```
void ClassA::doSomething(void (ClassA::*fn)(int, int), ClassA* classA) {
    (classA->fn)(3, 5);
}
```
Now, call function `doSomething` with
```
this->doSomething(ClassA::&aFunction, this);
```
