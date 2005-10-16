#ifndef _H_TeleopFsm
#define _H_TeleopFsm
#include <stddef.h>
#include "teleopcontext.h"
class TeleopFsm;

class TeleopFsmState {
public:

  virtual const char* StateName() const = 0;
  virtual void deactivate(TeleopFsm& s);
  virtual void activate(TeleopFsm& s);
};

class TeleopFsmActiveState : public TeleopFsmState {
public:
  virtual const char* StateName() const
  {return("Active");};
  virtual void deactivate(TeleopFsm&);
};

class TeleopFsmIdleState : public TeleopFsmState {
public:
  virtual const char* StateName() const
  {return("Idle");};
  virtual void activate(TeleopFsm&);
};
class TeleopFsm : public TeleopContext {
  public:
  static TeleopFsmActiveState ActiveState;
  static TeleopFsmIdleState IdleState;
  TeleopFsm();// default constructor
  void deactivate() {itsState->deactivate(*this);}
  void activate() {itsState->activate(*this);}
  void SetState(TeleopFsmState& theState) {itsState=&theState;}
  TeleopFsmState& GetState() const {return *itsState;};
  private:
    TeleopFsmState* itsState;
};
#endif
