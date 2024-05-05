#include "behavior_tree.h"

namespace tickles {

std::string_view ToString(Result r) {
    switch (r){
    case Result::Running:
      return "Running";
    case Result::Failed:
      return "Failed";
    case Result::Succeeded:
      return "Succeeded";
     default:
       return "<unknown>";
    }
  };
  
  std::ostream& operator<<(std::ostream& os, Result r) {
    os << ToString(r);
    return os;
  };

}
