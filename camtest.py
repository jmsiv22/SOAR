#pip install opencv-python requests numpy matplotlib torch torchvision torchaudio

import cv2
import requests
import numpy as np
import time
import matplotlib.pyplot as plt
import torch

url = "http://128.4.211.47/capture"  # Replace with ESP32's IP address

while True:
    try:
        # Get the image from the ESP32
        response = requests.get(url)
        image_data = np.frombuffer(response.content, np.uint8)
        frame = cv2.imdecode(image_data, cv2.IMREAD_COLOR)
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)
        results = model(frame_rgb)

        #display the image
        #print('Image')
        #print(frame_rgb)
        plt.imshow(frame_rgb)
        plt.axis('off')
        plt.pause(0.005)    # Pause briefly to update display
        plt.clf()   # Clear the figure for the next frame

        # Optional delay to control frame rate
        time.sleep(0.01)
    except Exception as e:
        print(f"Error fetching image: {e}")
        time.sleep(0.01)
