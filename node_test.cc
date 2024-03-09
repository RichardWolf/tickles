#include "gtest/gtest.h"
#include "node.h"

using tickles::AlwaysRunning;
using tickles::AlwaysSucceeded;
using tickles::AlwaysFailed;
using tickles::Parallel;
using tickles::Sequence;
using tickles::FallBack;

using tickles::Result;

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
  EXPECT_EQ(Parallel<>()(), Result::Succeeded);
  EXPECT_EQ(Parallel<AlwaysFailed>()(), Result::Failed);
  EXPECT_EQ(Parallel<AlwaysSucceeded>()(), Result::Succeeded);
  EXPECT_EQ(Parallel<AlwaysRunning>()(), Result::Running);
  EXPECT_EQ((Parallel<AlwaysRunning, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((Parallel<AlwaysRunning, AlwaysRunning, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((Parallel<AlwaysSucceeded, AlwaysRunning, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((Parallel<AlwaysRunning, AlwaysSucceeded, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((Parallel<AlwaysRunning, AlwaysRunning, AlwaysSucceeded>()()), Result::Running);
  EXPECT_EQ((Parallel<AlwaysFailed, AlwaysRunning, AlwaysRunning>()()), Result::Failed);
  EXPECT_EQ((Parallel<AlwaysRunning, AlwaysFailed, AlwaysRunning>()()), Result::Failed);
  EXPECT_EQ((Parallel<AlwaysRunning, AlwaysRunning, AlwaysFailed>()()), Result::Failed);
}

TEST(Nodes, Sequence) {
  EXPECT_EQ(Sequence<>()(), Result::Succeeded);
  EXPECT_EQ(Sequence<AlwaysFailed>()(), Result::Failed);
  EXPECT_EQ(Sequence<AlwaysSucceeded>()(), Result::Succeeded);
  EXPECT_EQ(Sequence<AlwaysRunning>()(), Result::Running);
  
  EXPECT_EQ((Sequence<AlwaysRunning, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((Sequence<AlwaysRunning, AlwaysFailed>()()), Result::Running);
  EXPECT_EQ((Sequence<AlwaysRunning, AlwaysSucceeded>()()), Result::Running);

  EXPECT_EQ((Sequence<AlwaysFailed, AlwaysRunning>()()), Result::Failed);
  EXPECT_EQ((Sequence<AlwaysFailed, AlwaysFailed>()()), Result::Failed);
  EXPECT_EQ((Sequence<AlwaysFailed, AlwaysSucceeded>()()), Result::Failed);

  EXPECT_EQ((Sequence<AlwaysSucceeded, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((Sequence<AlwaysSucceeded, AlwaysFailed>()()), Result::Failed);
  EXPECT_EQ((Sequence<AlwaysSucceeded, AlwaysSucceeded>()()), Result::Succeeded);

  EXPECT_EQ((Sequence<AlwaysSucceeded, AlwaysSucceeded, AlwaysRunning>()()), Result::Running);	
  EXPECT_EQ((Sequence<AlwaysSucceeded, AlwaysSucceeded, AlwaysFailed>()()), Result::Failed);
  EXPECT_EQ((Sequence<AlwaysSucceeded, AlwaysSucceeded, AlwaysSucceeded>()()), Result::Succeeded);
}

TEST(Nodes, Fallback) {
  EXPECT_EQ(FallBack<>()(), Result::Failed);
  EXPECT_EQ(FallBack<AlwaysFailed>()(), Result::Failed);
  EXPECT_EQ(FallBack<AlwaysSucceeded>()(), Result::Succeeded);
  EXPECT_EQ(FallBack<AlwaysRunning>()(), Result::Running);
  
  EXPECT_EQ((FallBack<AlwaysRunning, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((FallBack<AlwaysRunning, AlwaysFailed>()()), Result::Running);
  EXPECT_EQ((FallBack<AlwaysRunning, AlwaysSucceeded>()()), Result::Running);

  EXPECT_EQ((FallBack<AlwaysFailed, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((FallBack<AlwaysFailed, AlwaysFailed>()()), Result::Failed);
  EXPECT_EQ((FallBack<AlwaysFailed, AlwaysSucceeded>()()), Result::Succeeded);

  EXPECT_EQ((FallBack<AlwaysSucceeded, AlwaysRunning>()()), Result::Succeeded);
  EXPECT_EQ((FallBack<AlwaysSucceeded, AlwaysFailed>()()), Result::Succeeded);
  EXPECT_EQ((FallBack<AlwaysSucceeded, AlwaysSucceeded>()()), Result::Succeeded);


  EXPECT_EQ((FallBack<AlwaysFailed, AlwaysFailed, AlwaysRunning>()()), Result::Running);
  EXPECT_EQ((FallBack<AlwaysFailed, AlwaysFailed, AlwaysFailed>()()), Result::Failed);
  EXPECT_EQ((FallBack<AlwaysFailed, AlwaysFailed, AlwaysSucceeded>()()), Result::Succeeded);
}

