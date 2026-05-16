import numpy as np
from numpy import ndarray
import matplotlib.pyplot as plt

EPOCHES = 3000
LEARNING_RATE = 3e-5


def calc_loss(x: ndarray, y: ndarray, w: ndarray) -> int:
    p = np.dot(x, w)
    loss = np.mean(np.square(y - p))
    return loss


calc_w = np.array(((578783 / 1783961,), (975984 / 1783961,)))


x = np.array(((160, 20), (165, 23), (170, 21), (175, 42), (180, 35)))
print(x.shape)
y = np.array(
    (
        (60,),
        (72,),
        (65,),
        (80,),
        (76,),
    )
)
print(y.shape)

w = np.random.random((2, 1))

for i in range(EPOCHES):
    loss = 0
    grads = np.zeros_like(w)
    for idx in range(x.shape[0]):
        cur_x = x[idx : idx + 1, :]
        cur_y = y[idx : idx + 1, :]

        p = np.dot(cur_x, w)

        loss += np.square(cur_y - p)
        grads += 2 * (p - cur_y) * cur_x.T
    loss /= 5
    grads /= 5
    print(f"Epoch: {i}\tLoss:{loss}")
    w -= grads * LEARNING_RATE

print(
    f"Final gradient loss: {calc_loss(x,y,w)}\nFinal formula loss:  {calc_loss(x,y,calc_w)}"
)

x_height = x[:, 0]
y_weight = y[:, 0]

x_space = np.linspace(155, 185, 100)
mean_second_feature = np.mean(x[:, 1])
y_pred = w[0, 0] * x_space + w[1, 0] * mean_second_feature

y_formula = calc_w[0, 0] * x_space + calc_w[1, 0] * mean_second_feature

plt.figure(figsize=(8, 5))
plt.scatter(x_height, y_weight, color="red", s=100, label="Data")
plt.plot(x_space, y_pred, color="blue", linewidth=2, label="Gradient")
plt.plot(x_space, y_formula, color="green", linewidth=2, label="Formula")
plt.title("Linear Regression")
plt.xlabel("Height")
plt.ylabel("Weight")
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()
plt.show()
