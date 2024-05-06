#ifndef TICKLES_NODE2_H
#define TICKLES_NODE2_H

#include <optional>
#include <ios>
#include <string>
#include <string_view>
#include <tuple>

namespace tickles {
  
  enum class Result {Running, Succeeded, Failed};

  std::string_view ToString(Result r);
  
  std::ostream& operator<<(std::ostream& os, Result r);


  struct AlwaysRunning{ Result operator()() const {return Result::Running;}};
  struct AlwaysSucceeded{ Result operator()() const {return Result::Succeeded;}};
  struct AlwaysFailed{ Result operator()() const {return Result::Failed;}};


  template <typename T>
  concept BehaviorTreeNode = requires (T t) {Result{t()};};

  template<BehaviorTreeNode... Children> 
  class Parallel {
  public:
    Parallel(Children&&... children): children(std::forward<Children>(children)...){}
    Parallel(Parallel const&) = default;
    Parallel(Parallel&&) = default;
    
    Result operator()() const {
      return in_parallel<0>();
    }
  private:
      
    template<int i>
    Result in_parallel() const requires(i >= sizeof...(Children)) {
      return Result::Succeeded;
    }
    template<int i>
    Result in_parallel() const requires (i < sizeof...(Children)) {
      Result ith_result = std::get<i>(children)();
      if (ith_result == Result::Failed) return Result::Failed;
      Result rest_result = in_parallel<i+1>();
      return rest_result == Result::Failed ? Result::Failed:
	ith_result == Result::Succeeded && rest_result == Result::Succeeded ? Result::Succeeded :
	Result::Running;
    }
    
    std::tuple<Children...> children;
  };

  
  template<BehaviorTreeNode... Children>
  class Sequence {
  public:
    Sequence(Children&&... children): children(std::forward<Children>(children)...){}
    Sequence(Sequence const&) = default;
    Sequence(Sequence &&) = default;

    Result operator()() const {
      return in_sequence<0>();
    }

  private:
    template<int i>
      Result in_sequence() const requires (i >= sizeof...(Children)) {
      return Result::Succeeded;
    }
    template<int i>
    Result in_sequence() const requires (i < sizeof...(Children)) {
      Result first_result = std::get<i>(children)();
      if (first_result == Result::Succeeded) return in_sequence<i+1>();
      return first_result;
    };

    std::tuple<Children...> children;
  };
  
  template<BehaviorTreeNode... Children>
  class FallBack {
  public:
    FallBack(Children&&... children): children(std::forward<Children>(children)...){}
    FallBack(FallBack const&) = default;
    FallBack(FallBack &&) = default;

    Result operator()() const {
      return fall_back<0>();
    }

  private:
    template <int i>
    Result fall_back() const requires(i >= sizeof...(Children)) {
      return Result::Failed;
    }
    template<int i>
    Result fall_back() const requires (i < sizeof...(Children)) {
      Result first_result = std::get<i>(children)();
      if (first_result == Result::Failed) return fall_back<i+1>();
      return first_result;
    }

    std::tuple<Children...> children;
  };

}

#endif
