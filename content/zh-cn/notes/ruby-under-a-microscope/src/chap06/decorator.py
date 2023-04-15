#!/usr/bin/env python3

import functools

def with_prefix(s: str = ""):
    def _deco(f):
        @functools.wraps(f)
        def _wrapped(*args, **kwargs):
            return s + f(*args, **kwargs)
        return _wrapped
    return _deco

class Professor:
    def __init__(self, first_name, middle_name, last_name):
        self.first_name = first_name
        self.middle_name = middle_name
        self.last_name = last_name

    @with_prefix("Prof. ")
    def name(self):
        ans = self.first_name
        if self.middle_name != "":
            ans += " " + self.middle_name
        ans += " " + self.last_name
        return ans

p = Professor("Gilbert", "", "Strang")
print(p.name())
