class Node:
    """Node of the shape tree"""


    def __init__(self, rule_func, *args, **kwargs):
        self.parent = None
        self.children = []

        self.SetRuleFunc(rule_func, *args, **kwargs)

    # C++ side GameObject
    gameobject = None

    def SetParent(self, parent):
        self.parent = parent

    def AddChildren(self, children):
        """Takes either single node or a list of nodes"""
        if type(children) is list:
            self.children.extend(children)
        else:
            self.children.append(children)

    def SetRuleFunc(self, rule_func, *args, **kwargs):
        self.rule_func = rule_func
        self.name = rule_func.__name__
        self.args = args
        self.kwargs = kwargs

    def RunRule(self):
        if self.rule_func is None:
            pass
        else:
            self.rule_func(*self.args, **self.kwargs)
