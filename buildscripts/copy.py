from shutil import copy
import sys, os

src = sys.argv[1]
dst = sys.argv[2]

os.makedirs(os.path.dirname(dst), exist_ok=True)
copy(src, dst)
