#pragma once
#include<iostream>
#include<windows.h>

auto out_handle=GetStdHandle(STD_OUTPUT_HANDLE);

void set_cursor(short,short);
void get_cursor(short *,short *);
void move_cursor(short,short);

void set_cursor(short x,short y)
{
    SetConsoleCursorPosition(out_handle,{x,y});
}

void get_cursor(short *x,short *y)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(out_handle,&info);
    *x=info.dwCursorPosition.X,*y=info.dwCursorPosition.Y;
}