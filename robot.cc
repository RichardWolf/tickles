#include <optional>
#include <cmath>

#include "gtest/gtest.h"

#include "node.h"
#include "boost/di.hpp"


using tickles::Result;
using tickles::Mutator;
using tickles::FallBack;
using tickles::Sequence;

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
  RobotAutonomy() : _bt(create_bt()){}

  void position(RobotPosition const& position)  {
    _position = position;
    _bt();
  }

  void charge(RobotCharge const& charge) {
    _charge = charge;
    _bt();
  }

  RobotMovement const& movement() const {
    return _movement;
  }
  
private:
  RobotBehaviorTree create_bt() {
    auto injector = di::make_injector(di::bind<RobotPosition>.to(_position),
				      di::bind<RobotCharge>.to(_charge),
				      di::bind<RobotChargingState>.to(_charging_state),
				      di::bind<RobotMovement>.to(_movement));
    return injector.create<RobotBehaviorTree>();
  }

  // Inputs
  RobotPosition _position{5, 4};
  RobotCharge _charge{1.0};

  // Internal State
  Mutable<RobotChargingState> _charging_state{false};

  // Outputs
  Mutable<RobotMovement>& _movement{0};
  
  // Behavior Tree
  RobotBehaviorTree _bt = create_bt();
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


#if 0
TEST_F(TestRobot, ReturningToCharge) {
  robot_position = {5, 4};
  robot_charge = {0.21};
  charging_state = {true};
  Robot robot = injector().create<Robot>();
  auto battery_ok = injector().create<BatteryOk>();
  EXPECT_EQ(battery_ok(), Result::Failed);
  EXPECT_EQ(robot(), Result::Running);
  EXPECT_EQ(charging_state, true);
  EXPECT_EQ(robot_movement, -5);
}


TEST_F(TestRobot, Charging) {
  robot_position = {0, 0};
  robot_charge = {0.21};
  charging_state = {true};
  Robot robot = injector().create<Robot>();
  auto battery_ok = injector().create<BatteryOk>();
  EXPECT_EQ(battery_ok(), Result::Failed);
  EXPECT_EQ(robot(), Result::Running);
  EXPECT_EQ(charging_state, true);
  EXPECT_EQ(robot_movement, 0);
}

TEST_F(TestRobot, FinishedCharging) {
  robot_position = {0, 0};
  robot_charge = {1.0};
  charging_state = {true};
  Robot robot = injector().create<Robot>();
  auto battery_ok = injector().create<BatteryOk>();
  EXPECT_EQ(battery_ok(), Result::Succeeded);
  EXPECT_EQ(robot(), Result::Running);
  EXPECT_EQ(charging_state, false);
  EXPECT_EQ(robot_movement, 10);
}
#endif
