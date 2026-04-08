import networkx as nx
import matplotlib.pyplot as plt

G = nx.DiGraph()
edges = [(1, 2), (1, 3), (2, 4), (3, 4), (4, 5)]
G.add_edges_from(edges)

try:
    order = list(nx.topological_sort(G))
    print(f"Топологический порядок:{ order}")
except nx.NetworkXUnfeasible:
    print("В графе есть цикл")

nx.draw(G, node_color="red")
plt.show()
