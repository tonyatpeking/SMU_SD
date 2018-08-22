import importlib
import traceback
import utils
import sys
import os
from zzz.shape_func import BuildTree, PrintTreeStr, OutputTreeToFile

root_func_name = 'Root'
max_layers = 100
max_layers_name = 'max_layers'
max_nodes = 2000
max_nodes_name = 'max_nodes'
ruleset_name = ''
ruleset_fullpath = ''
ruleset_mod = None
time_of_recent_change = 0


def GetRootFunc(mod):
    return getattr(mod, root_func_name)

def GetMaxLayers(mod):
    return getattr(mod, max_layers_name, max_layers )

def GetMaxNodes(mod):
    return getattr(mod, max_nodes_name, max_nodes )

def LoadRuleset(mod_name):
    global ruleset_name
    global ruleset_fullpath
    global time_of_recent_change
    global ruleset_mod
    ruleset_name = mod_name
    ruleset_fullpath = utils.GetScriptsFolder() + '\\' + ruleset_name + '.py'
    msg = ''
    try:
        ruleset_mod = importlib.import_module(ruleset_name)
        importlib.reload(ruleset_mod)

        root_func = GetRootFunc(ruleset_mod)
        layers = GetMaxLayers(ruleset_mod)
        nodes = GetMaxNodes(ruleset_mod)
        BuildTree(root_func, layers, nodes)
        
        PrintTreeStr()
        OutputTreeToFile()
        msg = 'Loaded ' + mod_name + ' Successfully'
    except:
        msg = traceback.format_exc()
    finally:
        time_of_recent_change = os.stat(ruleset_fullpath).st_mtime
        print(msg)
        return msg


def DidFileChange():
    return os.stat(ruleset_fullpath).st_mtime != time_of_recent_change
