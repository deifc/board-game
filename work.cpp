#include "engine.h"
#include <bits/stdc++.h>
using namespace std;
using namespace engine;

void work(displayObject* target){
    cout<<target->getName()<<endl;
}
bool f=false;
int deltax,deltay;
void ppp(displayObject* self){
    if(f){
        self->getparent().moveto(mouseX-deltax,mouseY-deltay);
    }
}
void begin1(displayObject* self){
    this_thread::sleep_for(chrono::milliseconds(2000));
    f=true;
    deltax=mouseX-self->getparent().getx();
    deltay=mouseY-self->getparent().gety();
}
void end1(displayObject* self){
    f=false;
}
void fg(displayObject* self){
    self->getparent().setVisible(false);
}
int main(){
    init();
    sprite window=stage.addSprite("f","g",{{"window.png"}});
    sprite fffff=window.addSprite("g","f",{{720,50,0x00ff00}});
    button btn_confirm=window.addButton("g","f",{{"btn1.png"},{"btn2.png"}},"workwork");
    button btn_cancel=window.addButton("g","f",{{"btn3.png"},{"btn4.png"}},"");
    btn_confirm.moveto(100,400);
    btn_cancel.moveto(400,400);
    btn_cancel.addEventListener(MOUSE_CLICK,fg);
    fffff.addEventListener(MOUSE_DOWN,begin1);
    fffff.addEventListener(ENTER_FRAME,ppp);
    fffff.addEventListener(MOUSE_UP,end1);
    run();
    return 0;
}//
/*
Todo                   ddl

engine             ... 11/17
    --完善文本框、按钮的支持
    --透明图片�?
玩法；地图；系统设计 ... 11/17
    --界面布局
    --游戏系统
    --24个块的内�?
美术资源            ... 11/22
    --24个块的贴�?
    --按钮贴图
    --文本框？
编写，调�?          ... 12/10

*/