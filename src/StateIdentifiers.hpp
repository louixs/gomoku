#ifndef __STATEIDENTIFIERS_H_
#define __STATEIDENTIFIERS_H_

namespace States {
  enum ID {
    None,
    Title,
    Menu,
    LocalGame,
    HostOnlineGame,
    SelectHost,
    JoinOnlineGame,
    Loading,
    Pause
  };
}


#endif // __STATEIDENTIFIERS_H_
