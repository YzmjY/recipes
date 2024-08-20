#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>

namespace calculator {
class error : public std::runtime_error {
 public:
  error(const std::string& expr, const std::string& msg)
      : std::runtime_error(msg), expr_(expr) {}

  std::string expr() const { return expr_; }

 private:
  std::string expr_;
};

namespace {
class Caculator {
  enum class OpKind {
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
  };

  struct Operator {
    OpKind op;
    Associativity associativity;

    Operator(OpKind op, Associativity associativity)
        : op(op), associativity(associativity) {}
    int getPrecedence() {
      switch (op) {
        case OpKind::kAdd:
          return 1;
        case OpKind::kSub:
          return 1;
        case OpKind::kMul:
          return 2;
        case OpKind::kDiv:
          return 2;
        case OpKind::kMod:
          return 2;
        case OpKind::kPow:
          return 3;
        default:
          return 0;
      }
    }

    bool isNull() { return op == OpKind::kNull; }
  };

  struct OperatorValue {
    Operator op;
    int value;

    OperatorValue(Operator op, int value) : op(op), value(value) {}

    int getPrecedence() { return op.getPrecedence(); }

    bool isNull() { return op.isNull(); }
  };

 public:
  std::size_t pos_ = 0;
  std::string expr_;
  std::stack<OperatorValue> stk_;

  int Eval(const std::string& expr) {
    expr_ = expr;
    pos_ = 0;
    stk_ = std::stack<OperatorValue>();

    int ans = evalExpr();

    return ans;
  }

  void unexpected() const {
    std::ostringstream msg;
    msg << "Syntax error: unexpected token \""
        << expr_.substr(pos_, expr_.size() - pos_) << "\" at index " << pos_;
    throw calculator::error(expr_, msg.str());
  }

  void divideByZero() const {
    std::ostringstream msg;
    msg << "Syntax error: divide by zero at index " << pos_;
    throw calculator::error(expr_, msg.str());
  }
  int evalExpr() {
    skipSpace();

    stk_.push(OperatorValue(Operator(OpKind::kNull, Associativity::kLeft), 0));
    int val = parseValue();
    while (!stk_.empty()) {
      auto op = parseOperator();
      while (stk_.top().getPrecedence() > op.getPrecedence() ||
             (stk_.top().getPrecedence() == op.getPrecedence() &&
              stk_.top().op.associativity == Associativity::kLeft)) {
        // 两个哨兵NULL相遇，计算完成
        if (stk_.top().isNull() && op.isNull()) {
          stk_.pop();
          return val;
        }
        // 栈顶的计算优先级高于当前计算符的，可以进行计算
        auto top = stk_.top();
        val = calculate(top.value, val, top.op.op);
        stk_.pop();
      }

      stk_.push(OperatorValue(op, val));
      val = parseValue();
    }
    return 0;
  }

  int parseValue() {
    skipSpace();

    int val = 0;
    switch (getChar()) {
      case '(':
        pos_++;
        val = evalExpr();
        skipSpace();
        if (getChar() != ')') {
          unexpected();
        }
        pos_++;
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        val = getInt();
        break;
      case '+':
        pos_++;
        val = parseValue();
        break;
      case '-':
        pos_++;
        val = parseValue();
        val = -val;
        break;
      default:
        unexpected();
        break;
    }
    return val;
  }

  int getInt() {
    int ans = 0;
    while (pos_ < expr_.size() && expr_[pos_] >= '0' && expr_[pos_] <= '9') {
      ans = ans * 10 + (expr_[pos_] - '0');
      pos_++;
    }
    return ans;
  }
  int getChar() {
    if (!isEnd()) {
      return expr_[pos_];
    }
    return 0;
  }

  bool isEnd() {
    skipSpace();
    return pos_ >= expr_.size();
  }

  Operator parseOperator() {
    skipSpace();

    if (pos_ < expr_.size() && expr_[pos_] == '+') {
      pos_++;
      return Operator(OpKind::kAdd, Associativity::kLeft);
    } else if (pos_ < expr_.size() && expr_[pos_] == '-') {
      pos_++;
      return Operator(OpKind::kSub, Associativity::kLeft);
    } else if (pos_ < expr_.size() && expr_[pos_] == '*') {
      pos_++;
      return Operator(OpKind::kMul, Associativity::kLeft);
    } else if (pos_ < expr_.size() && expr_[pos_] == '/') {
      pos_++;
      return Operator(OpKind::kDiv, Associativity::kLeft);
    } else if (pos_ < expr_.size() && expr_[pos_] == '%') {
      pos_++;
      return Operator(OpKind::kMod, Associativity::kLeft);
    } else if (pos_ < expr_.size() && expr_[pos_] == '^') {
      pos_++;
      return Operator(OpKind::kPow, Associativity::kRight);
    } else {
      return Operator(OpKind::kNull, Associativity::kLeft);
    }
  }

  void skipSpace() {
    while (pos_ < expr_.size() && expr_[pos_] == ' ') {
      pos_++;
    }
  }

  static int pow(int a, int b) {
    int ans = 1;
    int x = a;
    while (b > 0) {
      if (b & 1) {
        ans += x;
      }
      b >>= 1;
      x *= x;
    }

    return ans;
  }

  int calculate(int a, int b, OpKind op) {
    switch (op) {
      case OpKind::kAdd:
        return a + b;
      case OpKind::kSub:
        return a - b;
      case OpKind::kMul:
        return a * b;
      case OpKind::kDiv:
        if (b == 0) {
          divideByZero();
        }
        return a / b;
      case OpKind::kMod:
        return a % b;
      case OpKind::kPow:
        return pow(a, b);
      default:
        return 0;
    }
  }
};
}  // namespace

int eval(const std::string& expr) {
  int ans = 0;
  try {
    /* code */
    Caculator caculator;
    ans = caculator.Eval(expr);
  } catch (const std::exception& e) {
    std::cout << e.what() << '\n';
  }
  return ans;
}
}  // namespace calculator

int main() {
  // std::string expr = "(1+2)*3";
  // std::cout << calculator::eval(expr) << std::endl;

  // expr = "(1+2)*3+4";
  // std::cout << calculator::eval(expr) << std::endl;

  // expr = "(1+2)*3+4*5";
  // std::cout << calculator::eval(expr) << std::endl;

  std::string expr = "xx";
  std::cout << calculator::eval(expr) << std::endl;

  return 0;
}