#ifndef ENGINE
#define ENGINE
#include <bits/stdc++.h>

#define VERSION 1.0
namespace engine{
    const int MOUSE_CLICK=1,MOUSE_OVER=2,MOUSE_OFF=3,MOUSE_DOWN=4,MOUSE_UP=5;
    const int ENTER_FRAME=6;
    class _m_baseobj;
    class _m_sprite;
    class _m_textfield;
    class _m_button;
    class button;
    class sprite;
    class textField;
    class eventListener;
    class displayObject;
    class flxg{
        public:
        std::string buf;
        int width,height,color;
        flxg(std::string x);
        flxg(int width,int height,int color);
    };
    class displayObject{
        public:
        displayObject()=delete;
        displayObject(_m_baseobj* ptr);
        void moveto(int newx,int newy);
        void rotate(int rot);
        void setVisible(bool visible);
        sprite addSprite(std::string type,std::string name,std::vector<flxg> frames);
        button addButton(std::string type,std::string name,std::vector<flxg> frames,std::string title);
        textField addTextField(std::string type,std::string name,int color1,int color2);
        displayObject getparent();
        void removeEventListener(int type,std::function<void(displayObject*)> callback);
        int getx();
        int gety();
        int getWidth();
        int getHeight();
        int getAngle();
        int getCurrFrame();
        bool getVisible();
        std::list<eventListener>::iterator addEventListener(int type,std::function<void(displayObject*)> callback);
        std::string getType();
        std::string getName();
        //std::string getText();
        bool hitTestPoint(int x,int y);
        _m_baseobj* base;
    };
    class event{
        public:
        displayObject target;
        int delta;
    };
    class sprite:public displayObject{
        public:
        _m_sprite* self;
        void gotoframe(int frame);
        sprite(displayObject ttfa);
    };
    class textField:public displayObject{
        public:
        _m_textfield* self;
        std::string getText();
        void setText(std::string text);
        void setWidth(int width);
        void setHeight(int height);
        textField(displayObject ttfa);
    };
    class button:public displayObject{
        public:
        _m_button* self;
        void gotoframe(int frame);
        button(displayObject ttfa);
    };
    extern int mouseX,mouseY;
    extern displayObject stage;
    std::vector<displayObject> getChildrenByType(std::string type);
    displayObject getChildByName(std::string name);
    void init();
    void run();
    using listenerID = std::list<eventListener>::iterator;
};
#endif
