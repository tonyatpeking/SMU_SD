import os

GoUpDir = os.path.dirname

def GetZZZModuleFolder():
    return GoUpDir(os.path.realpath(__file__))

def GetScriptsFolder():
    return GoUpDir(GetZZZModuleFolder())

def GetDataFolder():
    return GoUpDir(GetScriptsFolder())

def GetEXEFolder():
    return GoUpDir(GetDataFolder())

