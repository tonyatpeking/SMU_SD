import os
import sys

#insert module dir to python search path
real_path = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(1,real_path)

__all__ = ["shape_func"]