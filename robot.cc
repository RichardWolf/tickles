#include <optional>
#include <cmath>

#include "gtest/gtest.h"

#include "behavior_tree.h"
#include "autonomy.h"
#include "mutable.h"
#include "boost/di.hpp"


using tickles::Result;
using tickles::Mutator;
using tickles::FallBack;
using tickles::Sequence;
using tickles::Mutable;
using tickles::MutableRegistry;
using tickles::Mutator;
using tickles::Autonomy;

constexpr int kRechargePosition = 0;
constexpr int kMaxSpeed = 5;
constexpr double kMinBattery = 0.2;

namespace di = boost::di;

struct Position {
  int position;
  int velocity;
};

struct Charge {
  operator double() const {return charge;}
  double charge = 1;
};

struct ChargingState {
  operator bool() const {return is_charging;}
  bool is_charging = false;
};

struct Movement {
  operator int() const {return velocity;}
  int velocity;
};

template <typename T>
const T& collar(const T& value, const T& min, const T& max) {
  return std::min(max, std::max(min, value));
}

struct MoveToRechargeStation {
  Position const& position;
  Mutator<Movement>& mut_movement;
  
  Result operator()() const {
    if (position.position == kRechargePosition) {
      mut_movement.set(Movement{.velocity=0});
      return Result::Running;
    }
    auto distance = -position.position;
    mut_movement.set(Movement{.velocity=collar(distance, -kMaxSpeed, kMaxSpeed)});

    return Result::Running;
  };
};

struct BatteryOk{
  Mutator<ChargingState> charging_state;
  Charge const& charge;
  
  Result operator()() const {
    if (charge >= 1.0) {
      charging_state.set(ChargingState{false});
      return Result::Succeeded;
    }
    if (charge.charge <= kMinBattery || charging_state.get()) {
      charging_state.set(ChargingState{true});
      return Result::Failed;
    }
    return Result::Succeeded;
  }
};

struct GoAboutBusiness {
  Mutator<Movement> movement;
  Position const& position;
  Result operator()() const {
    if (position.position < 500) {
      movement.set(Movement{.velocity = 10});
    }
    return Result::Running;
  }
};

struct EnsureBattery :
  tickles::FallBack<BatteryOk,
		    MoveToRechargeStation> {};

struct RobotBehaviorTree : 
  tickles::Sequence<EnsureBattery,
		    GoAboutBusiness> {};

struct RobotData {
  // Inputs
  std::shared_ptr<Position> position;
  std::shared_ptr<Charge> charge;
  
  // Outputs
  std::shared_ptr<const Mutable<Movement>> movement;
};

class RobotAutonomy : Autonomy<RobotData, RobotBehaviorTree> {
public:
  RobotAutonomy(){}
  RobotAutonomy(RobotAutonomy const&) = delete;
  RobotAutonomy(RobotAutonomy &&) = delete;
  
  void position(Position const& position)  {
    *data().position = position;
    sync();
  }

  void charge(Charge const& charge) {
    *data().charge = charge;
    sync();
  }

  Movement const& movement() const {
    return data().movement->get();
  }
};

struct TestRobot : testing::Test {
  RobotAutonomy robot;
};

TEST_F(TestRobot, BatteryFull) {
  robot.charge(Charge{1.0});
  robot.position({5,3});
  EXPECT_EQ(robot.movement(), 10);
}

TEST_F(TestRobot, BatteryLow) {
  robot.position({5, 4});
  robot.charge({0.199});
  EXPECT_EQ(robot.movement(), -5);
}

TEST_F(TestRobot, BatteryLowIsSticky) {
  robot.position({5, 4});
  robot.charge({0.199});
  robot.charge({0.201});
  EXPECT_EQ(robot.movement(), -5);
}

TEST_F(TestRobot, Charging) {
  robot.position({0, 0});
  robot.charge({0.19});
  EXPECT_EQ(robot.movement(), 0);
}

TEST_F(TestRobot, FinishedCharging) {
  robot.position({0, 0});
  robot.charge({0.1});
  robot.charge({1.0});
  EXPECT_EQ(robot.movement(), 10);
}
