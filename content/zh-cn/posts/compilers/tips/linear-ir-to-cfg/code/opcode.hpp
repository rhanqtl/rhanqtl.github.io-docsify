#ifndef OPCODE_H
#define OPCODE_H

enum class Opcode {
  ADD,
  SUB,
  MUL,
  DIV,

  EQ,
  LT,

  NOT,

  CBR,
  BR,
  CALL,

  LOAD,
  STORE,
};

#endif  // OPCODE_H
