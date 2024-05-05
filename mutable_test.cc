#include "mutable.h"
#include "boost/di.hpp"
#include "gtest/gtest.h"

namespace di = boost::di;

using namespace tickles;

TEST(Mutable, MutableSyncIsFalseWhenUnchanged) {
  auto injector = di::make_injector();
  
  auto mutable_int = injector.create<std::shared_ptr<Mutable<int>>>();
  auto mutator_int = injector.create<Mutator<int>>();

  EXPECT_EQ(0, mutator_int.get());
  EXPECT_EQ(0, mutable_int->get());

  EXPECT_EQ(false, mutable_int->sync());
}

TEST(Mutable, MutatorUnchangedUntilSync) {
  auto injector = di::make_injector();
  
  auto mutable_int = injector.create<std::shared_ptr<Mutable<int>>>();
  auto mutator_int = injector.create<Mutator<int>>();
  mutator_int.set(42);
  EXPECT_EQ(0, mutator_int.get());
  EXPECT_EQ(0, mutable_int->get());
  
  EXPECT_EQ(true, mutable_int->sync());
  EXPECT_EQ(42, mutator_int.get());
  EXPECT_EQ(42, mutable_int->get());
}


TEST(Mutable, MutatorSyncReturnsFalseWhenSetToEqualValue) {
  auto injector = di::make_injector();
  
  auto mutable_int = injector.create<std::shared_ptr<Mutable<int>>>();
  auto mutator_int = injector.create<Mutator<int>>();
  mutator_int.set(42);
  mutable_int->bsync();
  mutator_int.set(42);
  EXPECT_EQ(false, mutable_int->sync());
}

TEST(Mutable, CanSyncThroughRegistry) {
  auto injector = di::make_injector();
  
  auto mutable_int = injector.create<std::shared_ptr<Mutable<int>>>();
  auto mutator_int = injector.create<Mutator<int>>();
  auto registry = injector.create<std::shared_ptr<MutableRegistry>>();
  mutable_int->set(42);
  mutable_int->sync();
    
  EXPECT_EQ(false, registry->sync());
  
  mutator_int.set(43);
  EXPECT_EQ(true, registry->sync());
  EXPECT_EQ(43, mutator_int.get());
  EXPECT_EQ(43, mutable_int->get());
}

