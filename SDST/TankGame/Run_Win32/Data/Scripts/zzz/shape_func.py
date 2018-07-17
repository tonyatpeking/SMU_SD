from shape_tree import ShapeTree
from node import Node
from tree_grapher import TreeToStr
import utils
import os
import codecs

_tree = ShapeTree()


def BuildTree(rootFunction, max_layers, max_nodes):
    global _tree
    _tree = ShapeTree()
    _tree.SetMaxLayers(max_layers)
    _tree.SetMaxNodes(max_nodes)
    _tree.SetRootNode(Node(rootFunction))
    _tree.BuildTree()


def GetTreeStr():
    return TreeToStr(_tree.root)


def GetTree():
    return _tree


def PrintTreeStr():
    print(GetTreeStr())


def OutputTreeToFile(filepath=None):
    if filepath is None:
        filepath = utils.GetDataFolder() + "\\Generated\\tree.txt"

    with codecs.open(filepath, 'wb+', encoding='utf-8') as f:
        f.write(GetTreeStr())


def AddNode(function, *args, **kwargs):
    _tree.AddNode(function, *args, **kwargs)
