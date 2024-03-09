#ifndef TICKLES_NODE_H
#define TICKLES_NODE_H

namespace tickles {

  enum class Result {Running, Succeeded, Failed};
 
  struct AlwaysRunning{ Result operator()() const {return Result::Running;}};
  struct AlwaysSucceeded{ Result operator()() const {return Result::Succeeded;}};
  struct AlwaysFailed{ Result operator()() const {return Result::Failed;}};

  template<template<typename ...> typename CompositeT, typename...ChildNodes>
  CompositeT<ChildNodes...> make_composite() {
    return CompositeT<ChildNodes...>{};
  }

  template<typename... ChildNodes>
  class Parallel {
  public:
    Result operator()() const {
      return in_parallel(ChildNodes{}...);
    }

  private:
    Result in_parallel() const {return Result::Succeeded;}
    template<typename First, typename...Rest>
    Result in_parallel(const First&first, Rest... rest) const {
      Result first_result = first();
      if (first_result == Result::Failed) return first_result;
      Result rest_result = in_parallel(rest...);
      return
	rest_result == Result::Failed ? rest_result :
	(first_result == Result::Succeeded) && (rest_result == Result::Succeeded) ? Result::Succeeded :
	Result::Running;
    };
  };
    
  template<typename... ChildNodes>
  class Sequence {
  public:
    Result operator()() const {
      return in_sequence(ChildNodes{}...);
    }

  private:
    Result in_sequence() const {return Result::Succeeded;}
    template<typename First, typename...Rest>
    Result in_sequence(const First&first, Rest... rest) const {
      Result first_result = first();
      if (first_result != Result::Succeeded ) return first_result;
      return in_sequence(rest...);
    };
  };
  
  template<typename... ChildNodes>
  class FallBack {
  public:
    Result operator()() const {
      return fall_back(ChildNodes{}...);
    }

  private:
    Result fall_back() const {return Result::Failed;}
    template<typename First, typename...Rest>
    Result fall_back(const First&first, Rest... rest) const {
      Result first_result = first();
      if (first_result != Result::Failed ) return first_result;
      return fall_back(rest...);
    }
  };
}

#endif 
