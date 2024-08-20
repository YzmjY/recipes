#include <string>

namespace calculator {
enum class Operator {
  kAdd,   // +
  kSub,   // -
  kMul,   // *
  kDiv,   // /
  kMod,   // %
  kPow,   // ^
  kNull,  // null
};

enum class Associativity {
  kLeft,
  kRight,
}

int Precedence(Operator op) {
  switch (op) {
    case Operator::kAdd:
    case Operator::kSub:
      return 1;
    case Operator::kMul:
    case Operator::kDiv:
      return 2;
    case Operator::kPow:
      return 3;
    case Operator::kMod:
      return 4;
    default:
      return 0;
  }
}

struct OperatorValue {
  Operator op;
  int value;
};

}  // namespace calculator
