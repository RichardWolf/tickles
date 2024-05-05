#include <optional>
#include <cmath>

#include "gtest/gtest.h"

#include "behavior_tree.h"
#include "mutable.h"
#include "boost/di.hpp"


using tickles::Result;
using tickles::Mutator;
using tickles::FallBack;
using tickles::Sequence;
using tickles::Mutable;
using tickles::MutableRegistry;
using tickles::Mutator;

constexpr int kRechargePosition = 0;
constexpr int kMaxSpeed = 5;
constexpr double kMinBattery = 0.2;

namespace di = boost::di;

struct RobotPosition {
  int position;
  int velocity;
};

struct RobotCharge {
  operator double() const {return charge;}
  double charge = 1;
};

struct RobotChargingState {
  operator bool() const {return is_charging;}
  bool is_charging = false;
};

struct RobotMovement {
  operator int() const {return velocity;}
  int velocity;
};

template <typename T>
const T& collar(const T& value, const T& min, const T& max) {
  return std::min(max, std::max(min, value));
}

struct MoveToRechargeStation {
  RobotPosition const& position;
  RobotCharge const& charge;
  RobotMovement const& movement;
  Mutator<RobotMovement>& mut_movement;
  Mutator<RobotChargingState>& charging_state;
  
  Result operator()() const {
    if (position.position == kRechargePosition) {
      mut_movement.set(RobotMovement{.velocity=0});
      return Result::Running;
    }
    auto distance = -position.position;
    mut_movement.set(RobotMovement{.velocity=collar(distance, -kMaxSpeed, kMaxSpeed)});

    return Result::Running;
  };
};

struct BatteryOk{
  Mutator<RobotChargingState> charging_state;
  RobotCharge const& charge;
  
  Result operator()() const {
    if (charge >= 1.0) {
      charging_state.set(RobotChargingState{false});
      return Result::Succeeded;
    }
    if (charge.charge <= kMinBattery || charging_state.get()) {
      charging_state.set(RobotChargingState{true});
      return Result::Failed;
    }
    return Result::Succeeded;
  }
};

struct GoAboutBusiness {
  Mutator<RobotMovement> movement;
  RobotPosition const& position;
  Result operator()() const {
    if (position.position < 500) {
      movement.set(RobotMovement{.velocity = 10});
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


struct RobotAutonomy {
public:
  RobotAutonomy() : _data(init_data()) {}

  void position(RobotPosition const& position)  {
    *_data.position = position;
    sync();
  }

  void charge(RobotCharge const& charge) {
    *_data.charge = charge;
    sync();
  }

  RobotMovement const& movement() const {
    return _data.movement->get();
  }
  
private:
  struct data {
    // Inputs
    std::shared_ptr<RobotPosition> position;
    std::shared_ptr<RobotCharge> charge;

    // Outputs
    std::shared_ptr<const Mutable<RobotMovement>> movement;
    std::shared_ptr<MutableRegistry> mutable_registry;

    // Behavior Tree
    RobotBehaviorTree bt;
  } _data;
  
  data init_data() {
    return di::make_injector().create<data>();
  }

  void sync() {
    do {
      _data.bt();
    } while (_data.mutable_registry->sync());
  }
};

struct TestRobot : testing::Test {
  RobotAutonomy robot;
};

TEST_F(TestRobot, BatteryFull) {
  robot.charge(RobotCharge{1.0});
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
