from zzz.shape_func import *

max_layers = 100
max_nodes = 2000

def Root():
    AddNode(Branch1) 


def Branch1():
    AddNode(Branch2)
    AddNode(Branch3) 



def Branch2():
    AddNode(Branch3)
    AddNode(Branch4)


def Branch3():
    AddNode(Branch5)
    AddNode(Branch4)


def Branch4():
    AddNode(Branch5)
    AddNode(Branch5)
    AddNode(Branch5)


def Branch5():
    pass
