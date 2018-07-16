from zzz.shape_func import *


def Root():
    AddNode(Branch1)


def Branch1():
    AddNode(Branch2)
    AddNode(Branch2)
    AddNode(Branch3)


def Branch2():
    AddNode(Branch3)


def Branch3():
    AddNode(Branch4)
    AddNode(Branch4)


def Branch4():
    pass


BuildTree(Root)
tree = GetTree()
i = 1
PrintTreeStr()
