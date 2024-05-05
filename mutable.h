#ifndef TICKLES_MUTABLE_H
#define TICKLES_MUTABLE_H

#include <memory>
#include <unordered_set>

#include "boost/di.hpp"

namespace tickles {
  // Tracks all mutable values

  class MutableBase;

  class MutableRegistry {
  public:
    MutableRegistry() {}
    MutableRegistry(MutableRegistry const&) = delete;
    MutableRegistry(MutableRegistry &&) = delete;

    void add(MutableBase* a);
    void remove(MutableBase* a);
    bool sync();
  private:
    std::unordered_set<MutableBase*> _as;
  };

  class MutableBase {
  public:
    MutableBase(std::shared_ptr<MutableRegistry> registry);
    MutableBase(MutableBase const&) = delete;
    MutableBase(MutableBase &&) = delete;
    virtual ~MutableBase();
    virtual bool sync() = 0;

  private:
    std::shared_ptr<MutableRegistry> _registry;
  };
  
  template<typename T>
  class Mutable : public MutableBase {
  public:    
    Mutable(std::shared_ptr<MutableRegistry> registry) : MutableBase(registry) {}
    Mutable(const Mutable&) = delete;
    Mutable(Mutable&&) = delete;

    template <typename U>
    void set(U&& u) {
      if (u == _next) return;
      _dirty = true;
      _next = std::move(u);
    }

    T const& get() const {return _last;}
    
    bool sync() override {
      _last = _next;
      bool was_dirty = _dirty;
      _dirty = false;
      return was_dirty;
    }

  private:
    bool _dirty = false;
    T _last{}, _next{};
  };
  
  template<typename T>
  class Mutator {
  public:
    Mutator(std::shared_ptr<Mutable<T>> mut) : _mutable(std::move(mut)) {}

    Mutator(Mutator const&) = default;
    Mutator(Mutator &&) = default;
    
    template <typename U>
    void set(U&& u) const {_mutable->set(std::forward<U>(u));}

    T const& get() const {return _mutable->get();}
    
  private:
    std::shared_ptr<Mutable<T>> _mutable;
  };
}

#endif
