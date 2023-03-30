import graphviz

with open('temp.txt','r') as f:
    lines = f.readlines()

FA = graphviz.Digraph('finite_state_machine', filename='FA_graph', format='png')
FA.attr(rankdir='LR', size="16", dpi="300")

states = lines[0].split(' ')
states.pop()
FA.attr('node', shape='circle')
for s in states:
    FA.node(s)

end_states = lines[1].split(' ')
end_states.pop()
FA.attr('node', shape='doublecircle')
for s in states:
    FA.node(s)

transitions = lines[2].split('|');
transitions.pop(0)
for t in transitions:
    for i in range(1,len(t),2):
        if(t[i] == ' '):
            FA.edge(t[0], t[i+1], label="ε")
        else:
            FA.edge(t[0], t[i+1], label=t[i])

FA.view()