from node import Node
from collections import namedtuple

_indent = 4  # minimum 2
_branch = "├"
_vertical = "│"
_corner = "└"
_horizontal = "─"


def TreeToStr(root):
    return NodeToStr(root, 0, '')


def NodeToStr(node, idx, prepend):
    """recursively make node str"""
    is_last = IsLastChild(node, idx)
    my_prepend = ''
    child_prepend = ''
    if node.parent is None:
        pass
    elif is_last:
        my_prepend = prepend + _corner + _horizontal * (_indent - 2) + ' '
        child_prepend = prepend + ' ' * _indent
    else:
        my_prepend = prepend + _branch + _horizontal * (_indent - 2) + ' '
        child_prepend = prepend + _vertical + ' ' * (_indent - 1)
    my_str = my_prepend + node.name
    for index, child in enumerate(node.children):
        my_str += '\n' + NodeToStr(child, index, child_prepend)

    return my_str


def IsLastChild(node, idx):
    if node.parent is None:
        return True
    if len(node.parent.children) == idx + 1:
        return True
    return False
