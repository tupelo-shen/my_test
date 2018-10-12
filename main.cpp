#include <iostream>
 
struct Empty {};
struct Base { int a; };
struct Derived : Base { int b; };
struct Bit { unsigned bit: 1; };
 
int main()
{
    Empty e;
    Derived d;
    Base& b = d;
    [[maybe_unused]] Bit bit;
    int a[10];
    std::cout << "size of empty class: "              << sizeof e          << '\n'  // out->1
              << "size of pointer: "                  << sizeof &e         << '\n'  // out->4
//            << "size of function: "                 << sizeof(void())    << '\n'  // error
//            << "size of incomplete type: "          << sizeof(int[])     << '\n'  // error
//            << "size of bit field: "                << sizeof bit.bit    << '\n'  // error
              << "size of array of 10 int: "          << sizeof(int[10])   << '\n'  // out->40
              << "size of array of 10 int (2): "      << sizeof a          << '\n'  // out->40
              << "length of array of 10 int: "        << ((sizeof a) / (sizeof *a)) << '\n' // out->10
              << "length of array of 10 int (2): "    << ((sizeof a) / (sizeof a[0])) << '\n' // out->10
              << "size of the Derived: "              << sizeof d          << '\n'  // out->8
              << "size of the Derived through Base: " << sizeof b          << '\n'; // out->4
 
}