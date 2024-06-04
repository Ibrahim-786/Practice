#include<iostream>
/* use file utility to check header info*/

//to run this application

// g++ Header.cpp -o Header

// ./Header

// file header


int main()
{

 std::cout<<"Hello w\n";

int a =100;
 const int b = a;
 int *p = (int *)&b;
 *p = 200;

 std::cout<<*p<<std::endl;
    return 0;
}