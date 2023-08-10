#include<iostream>
#include<windows.h>
#include<conio.h>
#include"screen.hpp"
#include"edit.hpp"

using std::cin;
using std::cout;
using std::make_shared;

shared_ptr<Edit> edit=make_shared<Normal>();

int main(int argc,char *argv[])
{
    //AllocConsole();
    
    system("cls");
    while(1) 
    {
        //char ch=_getch();
        //cout<<ch;
        edit->get_input();
        //cout<<1<<'\n';
    }
    return 0;
}