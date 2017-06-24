#!/usr/bin/python3
import os
from sys import argv, exit
from struct import pack

bax_struct = "<4sLLL112s"

TOP_FSIZE = (400*240*2)
SUB_FSIZE = (320*240*2)

def error(str):
    print(str)
    exit(1)

if len(argv) < 4:
    error("Usage: makeanim.py <fps> <top> <bottom>")

try:
    top = open(argv[2], "rb")
    bottom = open(argv[3], "rb")
except Exception as e:
    error(e)

top_filesize = os.stat(top.fileno()).st_size
bottom_filesize = os.stat(bottom.fileno()).st_size

top_fcount = top_filesize / TOP_FSIZE
bottom_fcount = bottom_filesize / SUB_FSIZE

if ((top_filesize % TOP_FSIZE) != 0) or ((bottom_filesize % SUB_FSIZE) != 0):
    error("invalid size")

out = open("out.bax", "wb+")
out.write(pack(bax_struct, ("BAX0").encode(), int(argv[1]), int(top_fcount), int(bottom_fcount), "".encode()))

while top_fcount or bottom_fcount:
    if top_fcount:
        out.write(top.read(TOP_FSIZE))
        top_fcount = top_fcount - 1
    if bottom_fcount:
        out.write(bottom.read(SUB_FSIZE))
        bottom_fcount = bottom_fcount - 1

out.close()
print("Wrote to out.bax")
