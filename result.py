from ctypes import *

class result(Structure):
    _fields_ = [
        ("type", c_uint),
        ("nbyte", c_uint),
        ("param", c_ubyte*2),
        ("data", c_ubyte*8),
    ]
