#ifndef __EFFECT_H_
#define __EFFECT_H_
#include <vector>
#include <memory>

class Player;
class Effect {
public:
    virtual void apply(Player*) = 0;
    virtual void undo(Player*) = 0;
    virtual std::string getTitle() = 0;
};

class SpeedModifier: public Effect {
public:
    SpeedModifier(float m): Effect(), modifier(m) {};
    void apply(Player*);
    void undo(Player*);
    std::string getTitle();
    float modifier;
};

typedef std::vector<std::shared_ptr<Effect>> Effects;

#endif 
