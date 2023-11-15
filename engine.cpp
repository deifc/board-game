
#include "engine.h"
#include "threadpool.h"
#include <graphics.h>
using namespace std;
namespace engine{
    const int SPRITE=1,BUTTON=2,TEXTFIELD=3;
    int mouseX=-1,mouseY=-1;
    mutex flock;
    _m_baseobj* _stage;
    displayObject stage(nullptr);
    int modified;
    class eventListener{
        public:
        int type;
        std::function<void(displayObject*)> callback;
        displayObject target;
        eventListener(int _type,std::function<void(displayObject*)> _callback,displayObject target);
        void operator ()();
    };
    class _m_baseobj{
        public:
        int enginetype;
        std::string type,name;
        std::string text;
        _m_baseobj(int _enginetype,std::string _type,std::string _name);
        int x,y,width,height,angle,visible;
        _m_baseobj* parent;
        std::list<_m_baseobj*> children;
        std::list<eventListener> evtListeners;
        void updateinfo();
    };
    class _m_sprite:public _m_baseobj{
        public:
        std::vector<IMAGE> frames;
        int currentframe;
        _m_sprite(string type,string name,std::vector<flxg> framename);
    };
    class _m_button:public _m_baseobj{
        public:
        std::vector<IMAGE> frames;
        int currentframe;
        std::string title;
        _m_button(string type,string name,std::vector<flxg> framename,string title);
    };
    class _m_textfield:public _m_baseobj{
        public:
        int bkgcolor,fontcolor;
        std::string text;
        _m_textfield(string type,string name,int _bkgcolor,int _fontcolor);
    };
    LOGFONT font;
    IMAGE buf;
    //std::function<void()> enterframe;
    std::vector<std::pair<std::list<eventListener>*,
        std::list<eventListener>::iterator>> todelete;
    std::vector<std::future<void>> ftrcontainer;
    ThreadPool pool;
    void eventDispatcher();
    void redraw();
}
using namespace std;
using namespace engine;

flxg::flxg(string x){
    buf=x;
}
flxg::flxg(int _width,int _height,int _color){
    width=_width;
    height=_height;
    color=_color;
}
void engine::init(){
    pool.init();
    initgraph(1280,720);
    buf.Resize(1280,720);
    _stage=new _m_baseobj(0,"stage","stage");
    _stage->parent=nullptr;
    stage=_stage;
    gettextstyle(&font);
    strcpy(font.lfFaceName,"Arial");
    settextstyle(&font);
    thread dispatcher(engine::eventDispatcher);
    dispatcher.detach();
}
list<_m_baseobj*> dfs(_m_baseobj* root){
    list<_m_baseobj*> ret={root};
    for(auto it:root->children)ret.splice(ret.end(),dfs(it));
    return ret;
}
displayObject engine::getChildByName(string x){
    list<_m_baseobj*> ttfa=dfs(_stage);
    for(_m_baseobj* ptr:ttfa)
        if(ptr->name==x)return displayObject(ptr);
    assert(0);
    return nullptr;
}
engine::sprite::sprite(displayObject x):
displayObject(x.base){
    self=(_m_sprite*)x.base;
}
engine::button::button(displayObject x):
displayObject(x.base){
    self=(_m_button*)x.base;
}
engine::textField::textField(displayObject x):
displayObject(x.base){
    self=(_m_textfield*)x.base;
}
displayObject displayObject::getparent(){
    if(base->parent!=nullptr)return base->parent;
    else throw "Error:this display object is root";
}
void recurdraw(_m_baseobj* rt,int x,int y){
    //cout<<rt<<endl;
    if(!rt->visible)return;
    if(rt->enginetype==SPRITE){
        const _m_sprite* ttfa=(_m_sprite*)rt;
        const IMAGE *buf=&(ttfa->frames[ttfa->currentframe]);
        putimage(x+ttfa->x,y+ttfa->y,buf);
    }else if(rt->enginetype==BUTTON){
        const _m_button* ttfa=(_m_button*)rt;
        setbkmode(TRANSPARENT);
        putimage(x+ttfa->x,y+ttfa->y,&ttfa->frames[ttfa->currentframe]);
        RECT gyy={x+ttfa->x,y+ttfa->y,ttfa->width+x+ttfa->x,ttfa->height+y+ttfa->y};
        drawtext(ttfa->title.c_str(),&gyy,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }else if(rt->enginetype==TEXTFIELD){
        const _m_textfield* ttfa=(_m_textfield*)rt;
        setbkmode(TRANSPARENT);
    }
    for(_m_baseobj* ttfa:rt->children)
        recurdraw(ttfa,x+rt->x,y+rt->y);
}
void recurwork(_m_baseobj* rt){
    for(eventListener& ttfa:rt->evtListeners)
        if(ttfa.type==ENTER_FRAME)ttfa();
    for(_m_baseobj* ttfa:rt->children)
        recurwork(ttfa);
}
void updatexy(){
    RECT ttfa;
    POINT pt;
    GetWindowRect(GetHWnd(),&ttfa);
    GetCursorPos(&pt);
    mouseX=-ttfa.left+pt.x;
    mouseY=-ttfa.top+pt.y;
}
void engine::redraw(){
    while(1){
        clock_t framebegin=clock();
        updatexy();
        recurwork(_stage);
        for(auto &f:todelete)
            f.first->erase(f.second);
        todelete.clear();
        SetWorkingImage(&buf);
        setfillcolor(0x000000);
        fillrectangle(0,0,1000,800);
        recurdraw(_stage,0,0);
        SetWorkingImage();
        BeginBatchDraw();
        putimage(0,0,&buf);
        EndBatchDraw();
        for(int i=0;i<ftrcontainer.size();i++)
            if(ftrcontainer[i].wait_for(chrono::milliseconds(0))==future_status::ready)
                ftrcontainer.erase(ftrcontainer.begin()+i),i--;
        clock_t frameend=clock();
        std::this_thread::sleep_for(chrono::milliseconds(16-(frameend-framebegin)));
    }  
}
engine::_m_sprite::_m_sprite(string type,string name,vector<flxg> _frames):_m_baseobj(1,type,name){
    for(flxg x:_frames){
        IMAGE ttfa;
        if(x.buf!="")loadimage(&ttfa,x.buf.c_str());
        else{
            ttfa=IMAGE(x.width,x.height);
            SetWorkingImage(&ttfa);
            setfillcolor(x.color);
            fillrectangle(0,0,x.width,x.height);
            SetWorkingImage();
        }
        frames.push_back(ttfa);
    }//cout<<114<<endl;
    currentframe=0;
    updateinfo();
}
engine::_m_button::_m_button(string type,string name,vector<flxg> _frames,string _title):_m_baseobj(2,type,name){
    for(flxg x:_frames){
        IMAGE ttfa;
        if(x.buf!="")loadimage(&ttfa,x.buf.c_str());
        else{
            ttfa=IMAGE(x.width,x.height);
            SetWorkingImage(&ttfa);
            setfillcolor(x.color);
            fillrectangle(0,0,x.width,x.height);
            SetWorkingImage();
        }
        frames.push_back(ttfa);
    }
    title=_title;
    currentframe=0;
    updateinfo();
}
engine::_m_textfield::_m_textfield(string type,string name,int color1,int color2):_m_baseobj(3,type,name){
    bkgcolor=color1;
    fontcolor=color2;
}
sprite engine::displayObject::addSprite(std::string _type,std::string _name,std::vector<flxg> _frames)
{
    base->children.push_back(new _m_sprite(_type,_name,_frames));
    (*base->children.rbegin())->parent=this->base;
    return sprite((*base->children.rbegin()));
}
void button::gotoframe(int frame){
    this->self->currentframe=frame;
}
void sprite::gotoframe(int frame){
    this->self->currentframe=frame;
}
void toframe2(displayObject* ttfa){
    button gyy(*ttfa);
    gyy.gotoframe(1); 
    gyy.self->updateinfo();
}
void toframe1(displayObject* ttfa){
    button gyy(*ttfa);
    gyy.gotoframe(0);
    gyy.self->updateinfo();
}
button engine::displayObject::addButton(std::string type,std::string name,std::vector<flxg> _frames,string title){
    base->children.emplace_back(new _m_button(type,name,_frames,title));
    (*base->children.rbegin())->parent=this->base;
    button temp=button((*base->children.rbegin()));
    temp.addEventListener(MOUSE_OVER,toframe2);
    temp.addEventListener(MOUSE_OFF,toframe1);
    temp.addEventListener(MOUSE_DOWN,toframe1);
    temp.addEventListener(MOUSE_UP,toframe2);
    return temp;
}
textField engine::displayObject::addTextField(std::string type,std::string name,int color1,int color2){
    base->children.emplace_back(new _m_textfield(type,name,color1,color2));
    return textField((*base->children.rbegin()));
}
void engine::run(){
    thread drawer(engine::redraw);
    drawer.join();
}
void engine::eventDispatcher(){
    int prevtime=0;
    int mouseX=-1,mouseY=-1;
    while(1){
        ExMessage msg=getmessage(EX_MOUSE);
        flock.lock();
        list<_m_baseobj*> gyy=dfs(_stage);
        if(msg.message==WM_LBUTTONDOWN){
            prevtime=clock();
            for(auto it=gyy.rbegin();it!=gyy.rend();++it){
                for(eventListener& ttfa:(*it)->evtListeners)
                    if(ttfa.type==engine::MOUSE_DOWN&&ttfa.target.hitTestPoint(msg.x,msg.y))ttfa();
                if(displayObject(*it).hitTestPoint(msg.x,msg.y))break;
            }
        }else if(msg.message==WM_MOUSEMOVE){
            for(_m_baseobj* temp:gyy)
            for(eventListener& ttfa:temp->evtListeners){
                if(ttfa.type==engine::MOUSE_OVER&&!(ttfa.target.hitTestPoint(mouseX,mouseY))
                    &&ttfa.target.hitTestPoint(msg.x,msg.y)){
                    ttfa();
                }else if(ttfa.type==engine::MOUSE_OFF&&(ttfa.target.hitTestPoint(mouseX,mouseY))
                    &&!ttfa.target.hitTestPoint(msg.x,msg.y)){
                    ttfa();
                }
            }    
            mouseX=msg.x,mouseY=msg.y;
        }else if(msg.message==WM_LBUTTONUP){
            for(_m_baseobj* temp:gyy)
                for(eventListener& ttfa:temp->evtListeners){
                    if(ttfa.type==engine::MOUSE_UP&&ttfa.target.hitTestPoint(msg.x,msg.y)){
                        ttfa();
                    }else if(ttfa.type==engine::MOUSE_CLICK&&clock()-prevtime<0.1*CLOCKS_PER_SEC&&ttfa.target.hitTestPoint(msg.x,msg.y)){
                        ttfa();
                    }
                }
        }
        flock.unlock();
    }
}
engine::eventListener::eventListener(int _type,function<void(engine::displayObject*)> _callback,displayObject _target)
:type(_type),callback(_callback),target(_target){}
void engine::eventListener::operator ()(){
    //callback(&this->target);
    if(this->type!=ENTER_FRAME)cout<<"begin"<<endl;
    ftrcontainer.emplace_back(pool.submit(callback,&this->target));//(&this->target);
    if(this->type!=ENTER_FRAME)cout<<"end"<<endl;
    modified=1;
}
engine::_m_baseobj::_m_baseobj(int _enginetype,std::string _type,std::string _name)
{
    enginetype=_enginetype;
    type=_type;name=_name;
    x=y=angle=0;
    visible=true;
}
void _m_baseobj::updateinfo(){
    if(enginetype==SPRITE){
        _m_sprite* f=(_m_sprite*)this;
        width=f->frames[f->currentframe].getwidth();
        height=f->frames[f->currentframe].getheight();
    }else if(enginetype==BUTTON){
        _m_button* f=(_m_button*)this;
        width=f->frames[f->currentframe].getwidth();
        height=f->frames[f->currentframe].getheight();
    }else if(enginetype==TEXTFIELD){
        //to do
    }
}
engine::displayObject::displayObject(engine::_m_baseobj* x):base(x){}
void engine::displayObject::moveto(int nx,int ny){
    base->x=nx,base->y=ny;
    //cout<<"?"<<endl;
    modified=1;
}
void displayObject::rotate(int x){
    base->angle+=x;
    base->angle%=360;
    //rotateimage(&self->frames[self->currentframe],&self->frames[self->currentframe],(double)x/180*3.14159265358,0,true);
    modified=1;
    base->updateinfo();
}
void displayObject::setVisible(bool visible){
    base->visible=visible;
    base->updateinfo();
    modified=1;
}
list<eventListener>::iterator displayObject::addEventListener(int type,function<void(displayObject*)> callback){
    base->evtListeners.emplace_back(type,callback,*this);
    return --base->evtListeners.end();
}
void displayObject::removeEventListener(int type,std::function<void(displayObject*)> callback){
    for(auto it=base->evtListeners.begin();it!=base->evtListeners.end();++it)
        if(it->target.base==this->base&&it->type==type&&
        it->callback.target<void(displayObject*)>()==callback.target<void(displayObject*)>())
            todelete.push_back({&base->evtListeners,it});
}
int displayObject::getx(){
    return base->x;
}
int displayObject::gety(){
    return base->y;
}
int displayObject::getWidth(){
    return base->width;
}
int displayObject::getHeight(){
    return base->height;
}
int displayObject::getAngle(){
    return base->angle;
}
std::string displayObject::getType(){
    return base->type;
}
std::string displayObject::getName(){
    return base->name;
}
void getxy(_m_baseobj* rt,int &x,int &y){
    while(rt!=nullptr){
        x-=rt->x;
        y-=rt->y;
        rt=rt->parent;
    }
}
bool displayObject::getVisible(){
    return base->visible;
}
bool displayObject::hitTestPoint(int x,int y){
    if(!base->visible)return false;
    getxy(base,x,y);
    return 0<=x&&0<=y&&base->width>=x&&base->height>=y;
}