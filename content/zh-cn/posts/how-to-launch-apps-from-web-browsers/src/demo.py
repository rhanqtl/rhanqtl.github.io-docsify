# -*- coding: UTF-8 -*-

import argparse
import sys
import urllib.parse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--from-url", type=str)
    args = parser.parse_args()

    try:
        url = urllib.parse.urlparse(args.from_url)
        if url.hostname != "eval":
            raise Exception("unsupported action: {}".format(url.hostname))
        expr = url.query.split("&")[0].split("=", 1)[1]
        print("'{}' is {}".format(expr, eval(expr)))
    except Exception as exn:
        print(exn)

    input("\nPress ENTER key to exit")
