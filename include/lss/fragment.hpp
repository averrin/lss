#ifndef __FRAGMENT_H_
#define __FRAGMENT_H_

#include <string>
#include <map>
#include <variant>
#include <functional>
#include <memory>

#include "CinderPango.h"

typedef std::variant<int, float, bool, std::string> tpl_arg;

class State;
struct LssEvent {
    virtual ~LssEvent() {};
};
class Fragment {
public:
        Fragment(std::string t, std::map<std::string, tpl_arg> args);
        Fragment(std::string t);
        std::string render(State s);

        int index;
        int length;
        PangoRectangle rect;

        virtual ~Fragment();
        bool needRender = true;

protected:
        std::string template_str;
        std::map<std::string, tpl_arg> args;
};

class Floor : public Fragment {
public:
        Floor();
        bool needRender = true;
};

class Wall : public Fragment {
public:
        Wall();
        bool needRender = true;
};

class Hero : public Fragment {
public:
        Hero();
        bool needRender = true;
};

typedef std::vector<std::shared_ptr<Fragment>> Fragments;

#endif // __FRAGMENT_H_
