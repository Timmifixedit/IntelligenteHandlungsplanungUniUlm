# A3
1. Wandele negative Prädikate P(x) zu Not_P(x):
Für einen Operator o aus O:
```python
for p in pre_minus(o):
    not_p = create_negative_predicate(p)
    for o_ in O:
        if p in add(o_):
            o_.del += not_p
        if p in del(o_):
            o_.add += not_p
        if p in pre_minus(o_):
            o_.pre_minus += not_p
            o_.pre_minus -= p
```
2. Analog
