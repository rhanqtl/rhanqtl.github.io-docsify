#!/usr/bin/env python3

import enum
from dataclasses import dataclass
from typing import List

class Opcode(enum.Enum):
    CONSTANT = 0,
    ID = 1,

    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,


@dataclass
class Instr:
    opcode: Opcode
    args: list


@dataclass
class BasicBlock:
    label: str
    instructions: List[Instr]
    successors: List[BasicBlock]
    predecessors: List[BasicBlock]


def calc_reaching_definitions(CFG):

