#ifndef TICKLES_AUTONOMY_H
#define TICKLES_AUTONOMY_H

#include "mutable.h"
#include "boost/di.hpp"

namespace tickles {

  template<typename DataT, typename BehaviorTreeT>
  class Autonomy {
  public:
    Autonomy() : _impl(init()) {}
    Autonomy(Autonomy &&) = default;
    Autonomy(Autonomy const&) = default;
    
    void sync() {
      do {
	_impl.behavior_tree();
      } while (_impl.mutable_registry->sync());
    }

    DataT& data() {
      return _impl.data;
    }

    const DataT& data() const {
      return _impl.data;
    }
    
  private:
    struct impl {
      DataT data;
      BehaviorTreeT behavior_tree;
      std::shared_ptr<MutableRegistry> mutable_registry;
    };
    
    impl init() {
      return boost::di::make_injector().create<impl>();
    }
    
    impl _impl;
  };
  
} // namespace tickles

#endif
