#!/usr/bin/env pythonn3

from collections import defaultdict
from typing import List
import enum


class Opcode(enum.Enum):
    CONSTANT = 0,
    ID = 1,

    ADD = 10,
    SUB = 11,
    MUL = 12,
    DIV = 13,


def is_binary_operator(opcode: Opcode) -> bool:
    return Opcode.ADD <= opcode <= Opcode.DIV


class Instr:
    def __init__(self, opcode: Opcode, args: list):
        self.opcode = opcode
        self.args = args


def _make_counter():
    cnt = 0
    def call():
        ans = cnt
        cnt += 1
        return ans
    return call


def do_lvn(bb: List[Instr]) -> List[Instr]:
    """
    Perform local value numbering on a basic block and returns the transformed basic block.

    Args:
        - bb: the basic block to be transformed

    Returns:
        - the transformed basic block
    """
    counter = _make_counter()
    new_bb = []
    var2num = defaultdict(counter)
    val2num = {}
    for i in bb:
        if i.opcode == Opcode.CONSTANT:
            if i.args[0] not in val2num:
                val2num[i.args[0]] = counter()
        elif i.opcode == Opcode.ID:
            pass
        elif is_binary_operator(i.opcode):
            pass
        else:
            raise Exception("unrecognized opcode: {}".format(i.opcode))
