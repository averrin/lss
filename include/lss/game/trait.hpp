#ifndef __TRAIT_H_
#define __TRAIT_H_
#include <string>

struct Trait {
  std::string name;

  friend bool operator==(Trait &t1, const Trait &t2) {
    return t1.name == t2.name;
  }
  friend bool operator<(Trait &t1, const Trait &t2) {
    return t1.name < t2.name;
  }
};

#endif // __TRAIT_H_
