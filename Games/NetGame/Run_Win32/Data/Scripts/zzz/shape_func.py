import utils
import os
import codecs

from shape_tree import ShapeTree
from node import Node
from tree_grapher import TreeToStr

from zzzEngine import *
from zzzGame import * 

_tree = ShapeTree()


def GetCurrentNode():
    return _tree.GetCurrentNode()


def GetCurrentGameObject():
    node = GetCurrentNode()
    if node is not None:
        go = node.GetGameObject()
        return go
    return None


def GetCurrentTransform():
    go = GetCurrentGameObject()
    if go is not None:
        t = go.GetTransform()
        return t
    return None


def BuildTree(rootFunction, max_layers, max_nodes):
    global _tree
    _tree.DestroyGameObjects()  # just mark as should die, c++ will clean up
    SetRootGameObject(None)
    _tree = ShapeTree()
    _tree.SetMaxLayers(max_layers)
    _tree.SetMaxNodes(max_nodes)
    _tree.SetRootNode(Node(rootFunction))
    _tree.BuildTree()
    root_node = _tree.GetRootNode()
    go = root_node.GetGameObject()
    SetRootGameObject(go)

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


def Insert(shape):
    pass


def Cube(side_len=[1, 1, 1], pivot=[0, 0, 0]):
    current_node = GetCurrentNode()
    go = MakeCube(Vec3(*side_len), Vec3(*pivot))
    current_node.SetGameObject(go)
    if current_node.GetParent() is not None:
        parent_go = current_node.GetParent().GetGameObject()
        if parent_go is not None:
            go.SetParent(parent_go)


def Rotate(*eulerAngles):
    t = GetCurrentTransform()
    if t is None:
        return
    t.RotateLocalEuler(Vec3(*eulerAngles))


def Translate(*displacement):
    t = GetCurrentTransform()
    if t is None:
        return
    t.TranslateLocal(Vec3(*displacement))


def Scale(*scale):
    t = GetCurrentTransform()
    if t is None:
        return
    t.SetLocalScale(Vec3(*scale))
