import numpy as np
from sklearn.linear_model import LinearRegression

# studying hours, visited lessons, avg mark
X = np.array([[10, 30, 8.3], [15, 46, 9.0], [20, 63, 9.8], [7, 24, 7.3], [12, 27, 9.3]])


y = np.array([8, 9, 10, 7, 9])


reg = LinearRegression()
reg.fit(X, y)
print(reg.predict([[12, 38, 9.8]]))
