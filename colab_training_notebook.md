# Ultrasonic Super-Resolution ML Model - Google Colab

## Step 1: Upload Dataset
```python
from google.colab import files
uploaded = files.upload()
```

## Step 2: Load & Group Data
```python
import pandas as pd, numpy as np

df = pd.read_csv(list(uploaded.keys())[0])
frames, group = [], []

for _, row in df.iterrows():
    angle = int(row["Angle"])
    if angle == 0 and group:
        frames.append(group)
        group = []
    group.append((angle, int(row["Sensor"]), float(row["Distance_cm"])))
frames.append(group)
```

## Step 3: Convert to Sparse Grid
```python
def polar_to_grid(frame, grid_size=(4, 13)):
    g = np.full(grid_size, -1.0)
    for angle, sensor, dist in frame:
        g[sensor - 1, int(angle / 15)] = dist
    return g

X = np.array([polar_to_grid(f) for f in frames]).reshape(-1, 4, 13, 1) / 100.0
```

## Step 4: Create Super-Resolution Targets
```python
import cv2

y = np.array([
    cv2.resize(x.squeeze(), (52, 16), interpolation=cv2.INTER_CUBIC)
    for x in X * 100
]).reshape(-1, 16, 52, 1) / 100.0
```

## Step 5: Build and Train Model
```python
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, UpSampling2D, InputLayer

model = Sequential([
    InputLayer(input_shape=(4, 13, 1)),
    Conv2D(32, 3, activation='relu', padding='same'),
    UpSampling2D((2, 2)),
    Conv2D(32, 3, activation='relu', padding='same'),
    UpSampling2D((2, 2)),
    Conv2D(1, 3, activation='linear', padding='same')
])

model.compile(optimizer='adam', loss='mse')
model.fit(X, y, epochs=20, batch_size=16)
```

## Step 6: Save Model
```python
model.save("ultrasonic_superres_model.h5")
from google.colab import files
files.download("ultrasonic_superres_model.h5")
```
