from node import *


class ShapeTree:

    def __init__(self):
        self.root = None
        self.current_node = None
        self.current_layer = []
        self.next_layer = []
        self.max_layers = 100
        self.max_nodes = 2000
        self.layer_count = 0
        self.node_count = 0

    def GetRootNode(self):
        return self.root

    def SetRootNode(self, node):
        self.root = node

    def GetCurrentNode(self):
        return self.current_node

    def SetCurrentNode(self, node):
        self.current_node = node

    def BuildTree(self):
        if self.root is None:
            return
        else:
            self.next_layer = []
            # not appended to current_layer because BuildNextLayer will advance to next layer
            self.next_layer.append(self.root)
            while len(self.next_layer) > 0:
                self.BuildNextLayer()

    def BuildNextLayer(self):
        self.current_layer = self.next_layer
        self.next_layer = []
        self.layer_count += 1

        if self.layer_count >= self.max_layers:
            return

        for node in self.current_layer:
            self.current_node = node
            node.RunRule()

    def AddNode(self, function, *args, **kwargs):

        self.node_count += 1
        if self.node_count >= self.max_nodes:
            return

        new_node = Node(function, *args, **kwargs)
        self.current_node.AddChildren(new_node)
        new_node.SetParent(self.current_node)
        self.next_layer.append(new_node)

    def SetMaxLayers(self, max_layers):
        self.max_layers = max_layers

    def SetMaxNodes(self, max_nodes):
        self.max_nodes = max_nodes
