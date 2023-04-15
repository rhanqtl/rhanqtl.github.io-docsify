#!/usr/bin/env python3

from collections import (defaultdict, namedtuple)
from typing import List, Union

BinaryExpr = namedtuple("BinaryExpr", ["T", "L", "op", "R"])
VariableRef = namedtuple("VariableRef", ["T", "S"])

def do_value_numbering(bb: List[Union[BinaryExpr, VariableRef]]):
    counter = 0
    def _incr():
        ans = counter
        counter += 1
        return ans

    vns_by_operand = defaultdict(_incr)
    operands_by_vn = {}
    vns_by_expr = {}
    for i, tac in enumerate(bb):
        T, L, op, R = tac
        vn_lhs, vn_rhs = vns_by_operand[L], vns_by_operand[R]
        k = (vn_lhs, op, vn_rhs)
        if k in vns_by_expr:
            vn = vns_by_expr[k]
            src = operands_by_vn[vn]
            bb[i] =
        else:
            pass



if __name__ == "__main__":
    bb = [
        TAC("a", "b", "+", "c"),
        TAC("b", "a", '-', "d"),
        TAC("c", "b", '+', "c"),
        TAC("d", "a", '-', "d"),
    ]
    do_value_numbering(bb)
