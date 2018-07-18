from zzz.shape_func import *

max_layers = 100
max_nodes = 2000


def Root():
    AddNode(MainMod)
    Cube([1, 1, 1])


def MainMod():
    Cube([15, 1, 1])
    AddNode(WingBase, dir=1)
    AddNode(WingBase, dir=-1)

    for xpos in [-7,-5, -3, 3, 5, 7]:
        AddNode(SolarOld, [260, 0, 0], [xpos, 0, 0])


def WingBase(dir):
    Cube([7, 0.7, 0.7], [-dir, 0, 0])
    Translate(7.5 * dir, 0, 0)

    AddNode(Solar, [0, 45, 0], [dir * 1, 1, 0])
    AddNode(Solar, [0, 45, 0], [dir * 5, 1, 0])
    AddNode(Solar, [0, 45, 180], [dir * 1, -1, 0])
    AddNode(Solar, [0, 45, 180], [dir * 5, -1, 0])

    AddNode(SolarOld, [90, 0, 0], [dir * 0, 0, 0])
    AddNode(SolarOld, [90, 0, 0], [dir * 6, 0, 0])


def Solar(r, t, s=[1, 1, 1]):
    Cube([4, 10, 0.1], [0, -1, 0])
    Scale(*s)
    Translate(*t)
    Rotate(*r)


def SolarOld(r, t, s=[1, 1, 1]):
    Cube([1.3, 8, 0.3], [0, -1, 0])
    Scale(*s)
    Translate(*t)
    Rotate(*r)
