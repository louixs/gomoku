#include "StateStack.hpp"

#include <cassert>

#include <iostream>
using namespace std;

StateStack::StateStack(State::Context context)
: mStack()
, mPendingList()
, mContext(context)
, mFactories()
{}

void StateStack::update(sf::Time dt) {
  // Iterate from top to bottom, stop as soon as update() returns false
  // using reverse iterator to start from the top
  cout << "StateStack::update" << endl;
  cout << "StateStack::update stack size: " << mStack.size() << endl;

  if (mStack.size() > 0) {

    for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr) {
      cout << "StateStack::update iterating" << endl;
      if (!(*itr)->update(dt)) {
        break;
      }
    }
  }
  
  applyPendingChanges();
  std::cout << "StateStack::update end" << endl;
}

void StateStack::draw() {
  if (mStack.size() > 0) {
    for (State::Ptr& state : mStack) {
      state->draw();
    }
  }
}

void StateStack::handleEvent(const sf::Event& event) {
  cout << "StateStack::handleEvent" << endl;
  if (mStack.size() > 0) {
    for (auto itr = mStack.rbegin(); itr != mStack.rend(); ++itr) {
      cout << "StateStack::handleEvent iterating" << endl;
      if (!(*itr)->handleEvent(event)) {
        break;
      }
    }
  }
  applyPendingChanges();
}

void StateStack::pushState(States::ID stateID) {
  mPendingList.push_back(PendingChange(Push, stateID));
  cout << "StateStack::pushState mPendingList size " << mPendingList.size() << endl;
}

void StateStack::popState() {
  mPendingList.push_back(PendingChange(Pop));
}

void StateStack::clearStates() {
  mPendingList.push_back(PendingChange(Clear));
}

bool StateStack::isEmpty() const {
  return mStack.empty();
}

State::Ptr StateStack::createState(States::ID stateID){
  cout << "StateStack::createState mFactories.size(): " << mFactories.size() << endl;
  cout << "stateID " << stateID << endl;
  auto found = mFactories.find(stateID);
  assert(found != mFactories.end());

  cout << "foud and first there: " << found->first << endl;
  return found->second();
}

void StateStack::applyPendingChanges(){
  cout << "StateStack::applyPendingChanges" << endl;
  cout << "mPendingList size " << mPendingList.size() << endl;
  for (PendingChange change : mPendingList) {
    cout << "mPendingList in the loop " << endl;
    switch (change.action) {
      
      case Push:
        cout << "Push" << endl;
        mStack.push_back(createState(change.stateID));
        cout << "StateStack::applyPendingChanges push, mStack size: " << mStack.size() << endl;
        break;

      case Pop:
        mStack.pop_back();
        break;

      case Clear:
        mStack.clear();
        break;
    }
  }

  mPendingList.clear();
}

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
: action(action)
, stateID(stateID)
{}
