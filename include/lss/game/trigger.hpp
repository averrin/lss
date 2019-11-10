#ifndef __TRIGGER_H_
#define __TRIGGER_H_
#include <functional>
#include <memory>

#include "lss/game/itemSpec.hpp"

typedef std::function<void()> TriggerCallback;
class Trigger {
    TriggerCallback callback;
public:
        Trigger(TriggerCallback cb) : callback(cb) {}
        void activate() {
            callback();
        }
    virtual ~Trigger() = default;
};

class EnterTrigger : public Trigger {
    public: EnterTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class SearchTrigger : public Trigger {
    public: SearchTrigger(TriggerCallback cb) : Trigger(cb) {}
};
class UseTrigger : public Trigger {
    public:
        UseTrigger(ItemSpec i, TriggerCallback cb) : Trigger(cb), item(i) {}
        ItemSpec item;
};

#endif // __TRIGGER_H_
