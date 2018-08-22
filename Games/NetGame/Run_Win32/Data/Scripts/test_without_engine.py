from zzz.ruleset_loader import LoadRuleset, DidFileChange

module_name = 'example_ruleset'

LoadRuleset(module_name)

while True:
    if DidFileChange():
        LoadRuleset(module_name)

