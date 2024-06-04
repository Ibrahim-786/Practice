#include<iostream>
#include<typeinfo>
using namespace std;
void fun()
{


}

int fun(int a)
{

return 0;
}
int main()
{

  cout<<"raw name of char\t"<<typeid(char).name()<<endl;
cout<<"raw name of int\t"<<typeid(int).name()<<endl;
fun();
    return 0;
}