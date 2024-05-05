#include "gtest/gtest.h"
#include "behavior_tree.h"
#include "boost/di.hpp"

using tickles::AlwaysRunning;
using tickles::AlwaysSucceeded;
using tickles::AlwaysFailed;
using tickles::Parallel;
using tickles::Sequence;
using tickles::FallBack;

using tickles::Result;

template <typename T>
T make() {
  return boost::di::make_injector().create<T>();
}
template <typename T>
Result eval() {
  return make<T>()();
}

TEST(Nodes, AlwaysRunning) {
  EXPECT_EQ(AlwaysRunning()(), Result::Running);
}

TEST(Nodes, AlwaysSucceeded) {
  EXPECT_EQ(AlwaysSucceeded()(), Result::Succeeded);
}

TEST(Nodes, AlwaysFailed) {
  EXPECT_EQ(AlwaysFailed()(), Result::Failed);
}

TEST(Nodes, Parallel) {
  EXPECT_EQ((eval<Parallel<>>()), Result::Succeeded);
  EXPECT_EQ((eval<Parallel<AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<Parallel<AlwaysSucceeded>>()), Result::Succeeded);
  EXPECT_EQ((eval<Parallel<AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Parallel<AlwaysRunning, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Parallel<AlwaysRunning, AlwaysRunning, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Parallel<AlwaysSucceeded, AlwaysRunning, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Parallel<AlwaysRunning, AlwaysSucceeded, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Parallel<AlwaysRunning, AlwaysRunning, AlwaysSucceeded>>()), Result::Running);
  EXPECT_EQ((eval<Parallel<AlwaysFailed, AlwaysRunning, AlwaysRunning>>()), Result::Failed);
  EXPECT_EQ((eval<Parallel<AlwaysRunning, AlwaysFailed, AlwaysRunning>>()), Result::Failed);
  EXPECT_EQ((eval<Parallel<AlwaysRunning, AlwaysRunning, AlwaysFailed>>()), Result::Failed);
}

TEST(Nodes, Sequence) {
  EXPECT_EQ((eval<Sequence<>>()), Result::Succeeded);
  EXPECT_EQ((eval<Sequence<AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<Sequence<AlwaysSucceeded>>()), Result::Succeeded);
  EXPECT_EQ((eval<Sequence<AlwaysRunning>>()), Result::Running);
  
  EXPECT_EQ((eval<Sequence<AlwaysRunning, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Sequence<AlwaysRunning, AlwaysFailed>>()), Result::Running);
  EXPECT_EQ((eval<Sequence<AlwaysRunning, AlwaysSucceeded>>()), Result::Running);

  EXPECT_EQ((eval<Sequence<AlwaysFailed, AlwaysRunning>>()), Result::Failed);
  EXPECT_EQ((eval<Sequence<AlwaysFailed, AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<Sequence<AlwaysFailed, AlwaysSucceeded>>()), Result::Failed);

  EXPECT_EQ((eval<Sequence<AlwaysSucceeded, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<Sequence<AlwaysSucceeded, AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<Sequence<AlwaysSucceeded, AlwaysSucceeded>>()), Result::Succeeded);

  EXPECT_EQ((eval<Sequence<AlwaysSucceeded, AlwaysSucceeded, AlwaysRunning>>()), Result::Running);	
  EXPECT_EQ((eval<Sequence<AlwaysSucceeded, AlwaysSucceeded, AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<Sequence<AlwaysSucceeded, AlwaysSucceeded, AlwaysSucceeded>>()), Result::Succeeded);
}

TEST(Nodes, Fallback) {
  EXPECT_EQ((eval<FallBack<>>()), Result::Failed);
  EXPECT_EQ((eval<FallBack<AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<FallBack<AlwaysSucceeded>>()), Result::Succeeded);
  EXPECT_EQ((eval<FallBack<AlwaysRunning>>()), Result::Running);
  
  EXPECT_EQ((eval<FallBack<AlwaysRunning, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<FallBack<AlwaysRunning, AlwaysFailed>>()), Result::Running);
  EXPECT_EQ((eval<FallBack<AlwaysRunning, AlwaysSucceeded>>()), Result::Running);

  EXPECT_EQ((eval<FallBack<AlwaysFailed, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<FallBack<AlwaysFailed, AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<FallBack<AlwaysFailed, AlwaysSucceeded>>()), Result::Succeeded);

  EXPECT_EQ((eval<FallBack<AlwaysSucceeded, AlwaysRunning>>()), Result::Succeeded);
  EXPECT_EQ((eval<FallBack<AlwaysSucceeded, AlwaysFailed>>()), Result::Succeeded);
  EXPECT_EQ((eval<FallBack<AlwaysSucceeded, AlwaysSucceeded>>()), Result::Succeeded);

  EXPECT_EQ((eval<FallBack<AlwaysFailed, AlwaysFailed, AlwaysRunning>>()), Result::Running);
  EXPECT_EQ((eval<FallBack<AlwaysFailed, AlwaysFailed, AlwaysFailed>>()), Result::Failed);
  EXPECT_EQ((eval<FallBack<AlwaysFailed, AlwaysFailed, AlwaysSucceeded>>()), Result::Succeeded);
}

