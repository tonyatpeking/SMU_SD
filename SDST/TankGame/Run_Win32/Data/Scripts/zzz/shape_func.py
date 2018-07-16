from shape_tree import ShapeTree
from node import Node
from tree_grapher import TreeToStr

_tree = ShapeTree()


def BuildTree(rootFunction):
    _tree.SetRootNode(Node(rootFunction))
    _tree.BuildTree()


def GetTreeStr():
    return TreeToStr(_tree.root)

def GetTree():
    return _tree


def PrintTreeStr():
    print(GetTreeStr())


def AddNode(function, *args, **kwargs):
    _tree.AddNode(function, *args, **kwargs)
