#ifndef __EVENTREACTOR_H_
#define __EVENTREACTOR_H_

#include "EventBus.hpp"
#include "EventHandler.hpp"
#include "lss/LSSApp.hpp"
#include "lss/game/events.hpp"

class EventReactor : public eb::EventHandler<EquipCommandEvent>,
                     public eb::EventHandler<DropCommandEvent>,
                     public eb::EventHandler<UseCommandEvent>,
                     public eb::EventHandler<HelpCommandEvent>,
                     public eb::EventHandler<InventoryCommandEvent>,
                     public eb::EventHandler<HeroCommandEvent>,
                     public eb::EventHandler<LightCommandEvent>,
                     public eb::EventHandler<ThrowCommandEvent>,
                     public eb::EventHandler<ZapCommandEvent>,
                     public eb::EventHandler<StairEvent>,
                     public eb::EventHandler<HeroDiedEvent>,
                     public eb::EventHandler<LocationChangeEvent>,
                     public eb::EventHandler<PauseEvent>,
                     public eb::EventHandler<DirectionEvent>,
                     public eb::EventHandler<TargetEvent>,
                     public eb::EventHandler<AnimationEvent>,
                     public eb::EventHandler<EnemyTakeDamageEvent>,
                     public eb::EventHandler<HeroTakeDamageEvent>,
                     public eb::EventHandler<QuitCommandEvent> {
public:
  EventReactor(LSSApp *a) : app(a) {
    eb::EventBus::AddHandler<QuitCommandEvent>(*this);
    eb::EventBus::AddHandler<EquipCommandEvent>(*this);
    eb::EventBus::AddHandler<DropCommandEvent>(*this);
    eb::EventBus::AddHandler<UseCommandEvent>(*this);
    eb::EventBus::AddHandler<ZapCommandEvent>(*this);
    eb::EventBus::AddHandler<HelpCommandEvent>(*this);
    eb::EventBus::AddHandler<HeroCommandEvent>(*this);
    eb::EventBus::AddHandler<LightCommandEvent>(*this);
    eb::EventBus::AddHandler<ThrowCommandEvent>(*this);
    eb::EventBus::AddHandler<InventoryCommandEvent>(*this);
    eb::EventBus::AddHandler<StairEvent>(*this);
    eb::EventBus::AddHandler<HeroDiedEvent>(*this);
    eb::EventBus::AddHandler<LocationChangeEvent>(*this);
    eb::EventBus::AddHandler<PauseEvent>(*this);
    eb::EventBus::AddHandler<DirectionEvent>(*this);
    eb::EventBus::AddHandler<TargetEvent>(*this);
    eb::EventBus::AddHandler<AnimationEvent>(*this);
    eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*this);
    eb::EventBus::AddHandler<HeroTakeDamageEvent>(*this);
  }
  LSSApp *app;

  virtual void onEvent(QuitCommandEvent &e) override;
  virtual void onEvent(EquipCommandEvent &e) override;
  virtual void onEvent(HelpCommandEvent &e) override;
  virtual void onEvent(InventoryCommandEvent &e) override;
  virtual void onEvent(DropCommandEvent &e) override;
  virtual void onEvent(UseCommandEvent &e) override;
  virtual void onEvent(ZapCommandEvent &e) override;
  virtual void onEvent(HeroCommandEvent &e) override;
  virtual void onEvent(LightCommandEvent &e) override;
  virtual void onEvent(ThrowCommandEvent &e) override;
  virtual void onEvent(StairEvent &e) override;
  virtual void onEvent(HeroDiedEvent &e) override;
  virtual void onEvent(LocationChangeEvent &e) override;
  virtual void onEvent(PauseEvent &e) override;
  virtual void onEvent(DirectionEvent &e) override;
  virtual void onEvent(TargetEvent &e) override;
  virtual void onEvent(AnimationEvent &e) override;
  virtual void onEvent(EnemyTakeDamageEvent &e) override;
  virtual void onEvent(HeroTakeDamageEvent &e) override;

  bool slotCallback(std::shared_ptr<Object>);
  bool itemCallback(std::shared_ptr<Slot>, std::shared_ptr<Object>);
};

#endif // __EVENTREACTOR_H_
