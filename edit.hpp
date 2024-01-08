#pragma once
#include<iostream>
#include<fstream>
#include<conio.h>
#include<vector>
#include<string>
#include<memory>
#include<windows.h>
#include<stack>
#include<utility>
#include"screen.hpp"

using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::shared_ptr;
using std::ifstream;
using std::ofstream;
using std::make_shared;
using std::stack;
using std::pair;

class Edit;

extern shared_ptr<Edit> edit; 

const short normal_y=34;

class Edit
{
    public:    
    using text=vector<string>;
    using point=pair<short,short>;
    using history=pair<text,point>;
    Edit() {}
    Edit(const vector<string> &t,const string &fl,stack<history>u,stack<history>r):
        txt(t),file(fl),un(u),re(r) {}
    virtual void render()=0;
    virtual void get_input()=0;
    void move_cursor(short,short);    

    protected:
    vector<string> txt{""};
    string file;    
    stack<pair<text,point>> un;
    stack<pair<text,point>> re;
};

class Normal:public Edit
{
    public:
    using Edit::Edit;
    virtual void render() override;
    virtual void get_input() override;
    void del();
    void undo();
    void redo();
    void open();
    void save();
    void search(const string&);
};

class Insert:public Edit
{
    public:
    using Edit::Edit;
    virtual void render() override;
    virtual void get_input() override;
    void insert(char);
    
};

void Edit::move_cursor(short dx,short dy)
{
    short x,y;
    get_cursor(&x,&y);
    if(dx&&x+dx>=0&&x+dx<=txt[y].size()) x+=dx;
    if(dy&&y+dy>=0&&y+dy<txt.size())
    {
        y+=dy;
        if(x>txt[y].size()) x=txt[y].size();
    } 
    set_cursor(x,y);
}

void Insert::get_input()
{
    //cin.sync();
    render();
    char ch=_getch();
    if(ch)
    {
        
        if(ch==27) edit=make_shared<Normal>(txt,file,un,re);
        else insert(ch);
    }
    else
    {
        char ch2=_getch();
        int dx=0,dy=0;
        switch(ch2)
        {
            case 72:dy=-1;break;
            case 80:dy=1;break;
            case 75:dx=-1;break;
            case 77:dx=1;break;
            default:goto fc;
        }
        if(dx||dy) move_cursor(dx,dy);
    }
    fc:;
}

void Insert::insert(char ch)
{
    short x,y;
    get_cursor(&x,&y);
    un.push({txt,{x,y}});
    re=stack<history>();
    if(ch=='\r') 
    {
        if(txt[y].size())
        {
            txt.insert(txt.begin()+y+1,txt[y].substr(x,txt[y].size()-x));
            txt[y]=txt[y].substr(0,x);            
        }
        else txt.push_back(string(""));
        set_cursor(0,y+1);
        //exit(0);
    }
    else if(ch=='\b')
    {
        if(x==0&&y>0)
        {
            size_t pos=txt[y-1].size();
            txt[y-1]+=txt[y];
            txt.erase(txt.begin()+y);
            set_cursor(static_cast<short>(pos),y-1);
        }
        else if(x)
        {
            txt[y]=txt[y].substr(0,x-1)+txt[y].substr(x,txt[y].size()-x);
            set_cursor(x-1,y);
        }
    }
    else 
    {
        if(txt[y].size()>0)
        {
            if(x==txt[y].size()) txt[y]+=ch;
            else txt[y].insert(x,1,ch);           
        }
        else 
        {
            txt[y]+=ch;
        }
        set_cursor(x+1,y);
    }
    render();
}

void Insert::render()
{
    short x,y;
    get_cursor(&x,&y);
    system("cls");
    set_cursor(0,normal_y);
    cout<<"<INSERT>";
    set_cursor(0,0);
    for(const auto &str:txt) cout<<str<<'\n';
    set_cursor(x,y);
    fflush(stdout);
}

void Normal::get_input()
{
    render();
    short x=0,y=0;
    get_cursor(&x,&y);
    char ch=_getch();
    if(ch==':')
    {
        //render();
        string status;
        set_cursor(10,normal_y);
        cout<<ch;
        cin>>status;
        if(status=="q") 
        {
            system("cls");
            FreeConsole();
            exit(0);
        }
        else if(status=="w") 
        {
            save();
            set_cursor(x,y);
        }
        else if(status=="wq")
        {
            save();
            system("cls");
            FreeConsole();           
            exit(0);
        }
        else if(status=="o") 
        {
            cin>>file;
            open();
            set_cursor(0,0);
        }
        
    }
    else if(ch=='/')
    {
        set_cursor(10,normal_y);
        cout<<"/";
        string status;
        cin>>status;
        search(status);
        set_cursor(x,y);
    }
    else if(ch=='i') edit=make_shared<Insert>(txt,file,un,re);
    else if(ch=='x') del();
    else if(ch=='u') undo();
    else if(ch=='r') redo();
    else if(!ch)
    {
        char ch2=_getch();
        int dx=0,dy=0;
        switch(ch2)
        {
            case 72:dy=-1;break;
            case 80:dy=1;break;
            case 75:dx=-1;break;
            case 77:dx=1;break;
            default:goto fc;
        }
        if(dx||dy) move_cursor(dx,dy);
    }
    fc:;
}

void Normal::del()
{
    short x=0,y=0;
    get_cursor(&x,&y);
    un.push({txt,{x,y}});
    re=stack<history>();
    if(x<txt[y].size()) txt[y].erase(x,1);
    render();
}

void Normal::open()
{
    ifstream fl(file);
    string line;
    vector<string> tmp;
    while(getline(fl,line)) tmp.push_back(line);
    txt=tmp;
    fl.close();
    render();
    
}

void Normal::save()
{
    ofstream fl(file);
    for(const auto &str:txt) fl<<str<<'\n';
    fl.close();
}

void Normal::search(const string &word)
{
    int x=-1,y=0;
    char ch;
    do
    {
        x=txt[y].find(word,x+1);
        while(x==-1)
        {
            y=(y+1)%txt.size();
            x=txt[y].find(word,0);
        }
        set_cursor(x,y);
        ch=_getch();
    }while(ch=='n');
}

void Normal::render()
{
    short x,y;
    system("cls");
    get_cursor(&x,&y);
    set_cursor(0,0);
    for(const auto &str:txt) cout<<str<<'\n';
    set_cursor(0,normal_y);
    cout<<"<NORMAL>";
    set_cursor(x,y);
    fflush(stdout);
}

void Normal::undo()
{
    short x=0,y=0;
    get_cursor(&x,&y);
    if(un.empty()) return;
    re.push({txt,{x,y}});
    txt=un.top().first;
    set_cursor(un.top().second.first,un.top().second.second);
    un.pop();
    render();
}

void Normal::redo()
{
    short x=0,y=0;
    get_cursor(&x,&y);
    if(re.empty()) return;
    un.push({txt,{x,y}});    
    txt=re.top().first;
    set_cursor(re.top().second.first,re.top().second.second);
    re.pop();   
    render();
}
