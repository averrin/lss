#ifndef __EVENTREACTOR_H_
#define __EVENTREACTOR_H_

#include "EventBus.hpp"
#include "EventHandler.hpp"
#include "lss/LSSApp.hpp"
#include "lss/game/events.hpp"

class EventReactor : public eb::EventHandler<eb::Event>,
                     public eb::EventHandler<EquipCommandEvent>,
                     public eb::EventHandler<DropCommandEvent>,
                     public eb::EventHandler<UseCommandEvent>,
                     public eb::EventHandler<HelpCommandEvent>,
                     public eb::EventHandler<InventoryCommandEvent>,
                     public eb::EventHandler<CommitEvent>,
                     public eb::EventHandler<ZapCommandEvent>,
                     public eb::EventHandler<StairEvent>,
                     public eb::EventHandler<HeroDiedEvent>,
                     public eb::EventHandler<LocationChangeEvent>,
                     public eb::EventHandler<PauseEvent>,
                     public eb::EventHandler<DirectionEvent>,
                     public eb::EventHandler<QuitCommandEvent> {
public:
  EventReactor(LSSApp *a) : app(a) {
    eb::EventBus::AddHandler<eb::Event>(*this);
    eb::EventBus::AddHandler<QuitCommandEvent>(*this);
    eb::EventBus::AddHandler<EquipCommandEvent>(*this);
    eb::EventBus::AddHandler<DropCommandEvent>(*this);
    eb::EventBus::AddHandler<UseCommandEvent>(*this);
    eb::EventBus::AddHandler<ZapCommandEvent>(*this);
    eb::EventBus::AddHandler<HelpCommandEvent>(*this);
    eb::EventBus::AddHandler<InventoryCommandEvent>(*this);
    eb::EventBus::AddHandler<CommitEvent>(*this);
    eb::EventBus::AddHandler<StairEvent>(*this);
    eb::EventBus::AddHandler<HeroDiedEvent>(*this);
    eb::EventBus::AddHandler<LocationChangeEvent>(*this);
    eb::EventBus::AddHandler<PauseEvent>(*this);
    eb::EventBus::AddHandler<DirectionEvent>(*this);
  }
  LSSApp *app;

  virtual void onEvent(eb::Event &e) override;
  virtual void onEvent(QuitCommandEvent &e) override;
  virtual void onEvent(EquipCommandEvent &e) override;
  virtual void onEvent(HelpCommandEvent &e) override;
  virtual void onEvent(InventoryCommandEvent &e) override;
  virtual void onEvent(DropCommandEvent &e) override;
  virtual void onEvent(UseCommandEvent &e) override;
  virtual void onEvent(ZapCommandEvent &e) override;
  virtual void onEvent(CommitEvent &e) override;
  virtual void onEvent(StairEvent &e) override;
  virtual void onEvent(HeroDiedEvent &e) override;
  virtual void onEvent(LocationChangeEvent &e) override;
  virtual void onEvent(PauseEvent &e) override;
  virtual void onEvent(DirectionEvent &e) override;

  bool slotCallback(std::shared_ptr<Object>);
  bool itemCallback(std::shared_ptr<Slot>, std::shared_ptr<Object>);
};

#endif // __EVENTREACTOR_H_
