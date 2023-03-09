#!/usr/bin/env python3

import enum
from typing import List, Dict
from collections import defaultdict


class State(enum.Enum):
    WHITE = 0,
    GRAY  = 1,
    BLACK = 2,


def calc_dominators(G: Dict[str, List[str]]):
    states = defaultdict(lambda: State.WHITE)
    attrs: Dict[str, set] = {}

    def dfs(v: str, path: List[str]):
        states[v] = State.GRAY
        my_path = path + [v]
        if v not in attrs:
            attrs[v] = my_path
        else:
            attrs[v] = set(my_path) & set(attrs[v])
        for w in G[v]:
            if states[w] != State.WHITE:
                continue
            dfs(w, my_path)
        states[v] = State.BLACK

    dfs("entry", [])
    return attrs


if __name__ == "__main__":
    G = {
        "entry": ["loop"],
        "loop": ["body", "exit"],
        "body": ["then", "endif"],
        "then": ["endif"],
        "endif": ["loop"],
        "exit": []
    }
    print(calc_dominators(G))


