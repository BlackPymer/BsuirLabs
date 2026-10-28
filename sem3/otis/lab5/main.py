import tkinter as tk
from tkinter import ttk, messagebox
from tkinter import font as tkfont
import matplotlib.pyplot as plt
from matplotlib.patches import Patch
import networkx as nx


# --- Цветовая схема ---------------------------------------------------------
# Интерфейс
BG_APP = "#0F172A"          # общий фон окна
BG_SURFACE = "#1E293B"      # фон полей ввода и кнопок
BG_SURFACE_ALT = "#334155"  # фон недоступных (диагональных) полей
BORDER = "#475569"          # рамки полей и кнопок
TEXT = "#F8FAFC"            # основной текст
TEXT_MUTED = "#94A3B8"      # приглушённый текст
ACCENT = "#3B82F6"          # акцент: активные кнопки, выделение
ACCENT_ACTIVE = "#60A5FA"   # акцент при наведении
HEADER_COL = "#2563EB"      # заголовки столбцов (Z1…Zn)
HEADER_ROW = "#7C3AED"      # заголовки строк (Z1…Zn)

# Граф
GRAPH_NODE = "#1E293B"
GRAPH_NODE_TEXT = "#F8FAFC"
GRAPH_EDGE = "#64748B"

# Диаграмма весов
BAR_BEST = "#16A34A"        # лучшая цель
BAR_OTHER = "#64748B"       # остальные цели
BAR_TOTAL = "#EA580C"       # сумма весов


def apply_theme(root: tk.Tk) -> None:
    """Настраивает ttk-стили: у ttk-виджетов цвета задаются только через них."""
    style = ttk.Style(root)
    style.theme_use("clam")

    header_font = tkfont.nametofont("TkDefaultFont").copy()
    header_font.configure(size=10, weight="bold")

    style.configure(".", background=BG_APP, foreground=TEXT,
                    fieldbackground=BG_SURFACE, bordercolor=BORDER,
                    lightcolor=BORDER, darkcolor=BORDER,
                    troughcolor=BG_SURFACE, focuscolor=ACCENT)

    style.configure("TLabel", background=BG_APP, foreground=TEXT)
    style.configure("ColHeader.TLabel", background=HEADER_COL, foreground=TEXT,
                    padding=(8, 3), font=header_font)
    style.configure("RowHeader.TLabel", background=HEADER_ROW, foreground=TEXT,
                    padding=(8, 3), font=header_font)

    style.configure("TEntry", fieldbackground=BG_SURFACE, foreground=TEXT,
                    insertcolor=TEXT, selectbackground=ACCENT, selectforeground=TEXT,
                    padding=3)
    style.map("TEntry",
              fieldbackground=[("disabled", BG_SURFACE_ALT)],
              foreground=[("disabled", TEXT_MUTED)])

    style.configure("TButton", background=BG_SURFACE, foreground=TEXT,
                    bordercolor=BORDER, padding=(10, 5))
    style.map("TButton",
              background=[("pressed", ACCENT_ACTIVE), ("active", ACCENT)],
              foreground=[("active", TEXT)])

    style.configure("Accent.TButton", background=ACCENT, foreground=TEXT,
                    bordercolor=ACCENT, padding=(14, 5), font=header_font)
    style.map("Accent.TButton",
              background=[("pressed", ACCENT_ACTIVE), ("active", ACCENT_ACTIVE)])

    style.configure("TRadiobutton", background=BG_APP, foreground=TEXT,
                    indicatorcolor=BG_SURFACE, indicatorrelief="flat")
    style.map("TRadiobutton",
              background=[("active", BG_APP)],
              foreground=[("active", ACCENT_ACTIVE)],
              indicatorcolor=[("selected", ACCENT), ("active", ACCENT)])


class GUI:
    EXAMPLES = [
        {
            "name": "Пример 1",
            "desc": "Z1 — метро, Z2 — двухэтажный автобус, Z3 — расширение сети, Z4 — скоростной трамвай",
            "matrix": [["", "1", "1", "1"],
                       ["0", "", "0", "0"],
                       ["0", "1", "", "1"],
                       ["0", "1", "0", ""]],
        },
        {
            "name": "Пример 2",
            "desc": "Z1 и Z4 делят первое место по числу побед",
            "matrix": [["", "1", "1", "0"],
                       ["0", "", "1", "0"],
                       ["0", "0", "", "1"],
                       ["1", "1", "0", ""]],
        },
        {
            "name": "Пример 3",
            "desc": "Z2 выигрывает все парные сравнения",
            "matrix": [["", "0", "1", "1"],
                       ["1", "", "1", "1"],
                       ["0", "0", "", "0"],
                       ["0", "0", "1", ""]],
        },
    ]

    def __init__(self, num_variants=4):
        self.root = None
        self.btn = None
        self.entries = []
        self.screen_width = 800
        self.screen_height = 600
        self.num_variants = num_variants
        self.method_var = None

    def draw_meny(self, solve, load_example) -> None:
        self.root = tk.Tk()
        self.root.geometry(f"{self.screen_width}x{self.screen_height}")
        self.root.title("Main meny")
        self.root.configure(bg=BG_APP)
        apply_theme(self.root)

        n = self.num_variants
        for i in range(n):
            ttk.Label(self.root, text=f"Z{i + 1}",
                      style="ColHeader.TLabel").place(x=self.screen_width * i / n + 50, y=20,
                                                      anchor="center")
            ttk.Label(self.root, text=f"Z{i + 1}",
                      style="RowHeader.TLabel").place(x=20, y=self.screen_height * i / n + 50,
                                                      anchor="center")
            for j in range(n):
                entry = ttk.Entry(self.root, width=5)
                if i == j:
                    entry.configure(state="disabled")
                entry.place(x=self.screen_width * j / n + 50, y=self.screen_height * i / n + 50, anchor="center")
                self.entries.append(entry)

        self.method_var = tk.StringVar(value="pairwise")
        ttk.Radiobutton(self.root, text="Метод парных сравнений",
                        variable=self.method_var, value="pairwise").place(x=50, y=530, anchor="w")
        ttk.Radiobutton(self.root, text="Метод последовательных сравнений",
                        variable=self.method_var, value="sequential").place(x=280, y=530, anchor="w")

        for k, example in enumerate(self.EXAMPLES):
            ttk.Button(self.root, text=example["name"],
                       command=lambda e=example: load_example(e)).place(x=50 + 110 * k, y=565, anchor="w")

        self.btn = ttk.Button(self.root, text="Solve", command=solve,
                              style="Accent.TButton")
        self.btn.place(x=self.screen_width - 20, y=565, anchor="e")

        self.root.mainloop()

    def get_entries_text(self) -> list[str]:
        res = []
        for entry in self.entries:
            res.append(entry.get())
        return res

    def fill(self, matrix) -> None:
        n = self.num_variants
        for i in range(n):
            for j in range(n):
                if i == j:
                    continue
                entry = self.entries[i * n + j]
                entry.configure(state="normal")
                entry.delete(0, tk.END)
                entry.insert(0, matrix[i][j])


class Graph:
    def __init__(self, num_variants):
        self.num_variants = num_variants
        self.graph = nx.DiGraph()
        self.adj_matrix = []

    def set_adj_matrix(self, adj_matrix) -> None:
        n = self.num_variants
        self.adj_matrix = [[float(cell) if cell else 0.0 for cell in row] for row in adj_matrix]
        self.graph.clear()
        self.graph.add_nodes_from(range(n))
        for i in range(n):
            for j in range(n):
                if i == j:
                    continue
                weight = self.adj_matrix[i][j]
                if weight:
                    self.graph.add_edge(i, j, weight=weight)

    def count_edges(self) -> int:
        return self.graph.number_of_edges()

    def count_row_ones(self) -> list[int]:
        res = []
        for i, row in enumerate(self.adj_matrix):
            count = 0
            for j, weight in enumerate(row):
                if i == j:
                    continue
                if weight:
                    count += 1
            res.append(count)
        return res

    def weights(self) -> list[float]:
        counts = self.count_row_ones()
        total = sum(counts)
        if not total:
            return [0.0 for _ in counts]
        return [count / total for count in counts]

    def sequential_weights(self) -> tuple[list[float], list[float]]:
        counts = self.count_row_ones()
        order = sorted(range(len(counts)), key=lambda i: (-counts[i], i))
        top = counts[order[0]]
        scores = [100.0 * count / top for count in counts]
        for pos, i in enumerate(order[:-2]):
            for a in range(pos + 1, len(order) - 1):
                for b in range(a + 1, len(order)):
                    j, k = order[a], order[b]
                    if self.adj_matrix[i][j] and self.adj_matrix[i][k] and scores[i] <= scores[j] + scores[k]:
                        scores[i] = 1.25 * (scores[j] + scores[k])
        total = sum(scores)
        if not total:
            return scores, [0.0 for _ in scores]
        return scores, [score / total for score in scores]

    def preference_order(self, weights) -> list[int]:
        return sorted(range(len(weights)), key=lambda i: (-weights[i], i))

    def best_index(self, weights) -> int | None:
        if not weights or max(weights) == 0:
            return None
        return max(range(len(weights)), key=weights.__getitem__)

    def print_stats(self, method, weights, scores=None) -> None:
        counts = self.count_row_ones()
        print("Метод парных сравнений" if method == "pairwise" else "Метод последовательных сравнений")
        print(f"Всего побед в матрице (сумма C): {self.count_edges()}")
        for i, count in enumerate(counts):
            print(f"  Z{i + 1}: C={count}, V={weights[i]:.3f}")
        if scores is not None:
            print("Скорректированные оценки p:")
            for i, score in enumerate(scores):
                print(f"  p(Z{i + 1}) = {score:.1f}")
        print(f"Проверка, сумма весов: {sum(weights):.3f}")
        order = self.preference_order(weights)
        print("Порядок предпочтения: " + " > ".join(f"Z{i + 1}" for i in order))

    def draw(self) -> None:
        pos = nx.spring_layout(self.graph, seed=42, weight=None)
        nx.draw(self.graph, pos, with_labels=True, node_color=GRAPH_NODE,
                font_color=GRAPH_NODE_TEXT, edge_color=GRAPH_EDGE, arrowsize=15,
                node_size=900, font_size=11)

    def draw_stats(self, weights, method) -> None:
        total = sum(weights)
        best = self.best_index(weights)
        labels = [f"Z{i + 1}" for i in range(len(weights))] + ["Итого"]
        values = list(weights) + [total]
        colors = [BAR_BEST if i == best else BAR_OTHER for i in range(len(weights))]
        colors.append(BAR_TOTAL)
        title = "Метод парных сравнений" if method == "pairwise" else "Метод последовательных сравнений"

        fig, ax = plt.subplots(figsize=(7, 5))
        bars = ax.bar(labels, values, color=colors)
        ax.bar_label(bars, padding=3, fontsize=9, fmt=lambda v: f"{v:.2f}")
        ax.set_title(f"Веса целей. {title}", fontsize=12)
        ax.set_xlabel("Цели")
        ax.set_ylabel("Вес цели V")
        ax.set_ylim(0, max(values) + 0.1)
        ax.yaxis.grid(True, linestyle="--", alpha=0.4)
        ax.legend(handles=[
            Patch(color=BAR_BEST, label="Лучшая цель"),
            Patch(color=BAR_OTHER, label="Остальные цели"),
            Patch(color=BAR_TOTAL, label="Сумма весов (проверка: = 1)"),
        ], loc="upper left")
        fig.tight_layout()


class Main:
    def __init__(self):
        self.root = None
        self.gui = GUI()

    def start(self):
        self.gui.draw_meny(self.solve, self.load_example)

    def load_example(self, example):
        self.gui.fill(example["matrix"])

    def validate(self, values) -> list[list[str]] | None:
        n = self.gui.num_variants
        for k, cell in enumerate(values):
            i, j = divmod(k, n)
            if i == j:
                continue
            if cell.strip() not in ("", "0", "1"):
                messagebox.showerror("Ошибка ввода",
                                     f"Ячейка Z{i + 1} → Z{j + 1}: ожидается 0 или 1, введено «{cell}»")
                return None
        matrix = [[values[i * n + j].strip() for j in range(n)] for i in range(n)]
        counts = [sum(1 for j in range(n) if i != j and matrix[i][j] == "1") for i in range(n)]
        if sum(counts) == 0:
            messagebox.showerror("Ошибка ввода",
                                 "Нет ни одной победы: сумма цен всех целей равна 0, веса не определить")
            return None
        return matrix

    def solve(self):
        n = self.gui.num_variants
        values = self.gui.get_entries_text()
        matrix = self.validate(values)
        if matrix is None:
            return

        graph = Graph(n)
        graph.set_adj_matrix(matrix)

        method = self.gui.method_var.get()
        if method == "pairwise":
            weights = graph.weights()
            scores = None
        else:
            scores, weights = graph.sequential_weights()

        graph.print_stats(method, weights, scores)
        graph.draw()
        graph.draw_stats(weights, method)
        plt.show()


if __name__ == "__main__":
    main = Main()
    main.start()
