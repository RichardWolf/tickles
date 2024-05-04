#include "mutable.h"

#include <algorithm>

namespace tickles {
  
MutableBase::MutableBase(std::shared_ptr<MutableRegistry> registry) : _registry(std::move(registry)){
  _registry->add(this);
}

MutableBase::~MutableBase() {
  _registry->remove(this);
}

void MutableRegistry::add(MutableBase* a) {
  _as.insert(a);
}

void MutableRegistry::remove(MutableBase* a) {
  _as.erase(a);
}

bool MutableRegistry::sync() {
  return std::ranges::any_of(_as, [](MutableBase* mut) {return mut->sync();});
}

} // namespace tickles
