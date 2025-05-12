# This code is designed to run on Python version 3.9.13
# pip install opencv-python requests numpy torch torchvision ultralytics
import cv2
import requests
import numpy as np
import time
import torch
import warnings
import datetime

warnings.filterwarnings("ignore", category=FutureWarning)

# Load YOLOv5 model
url = "http://192.168.43.42/image"  # Replace with ESP32's IP address
device = torch.device('cuda:0' if torch.cuda.is_available() else 'cpu')
model = torch.hub.load('ultralytics/yolov5', 'yolov5n', pretrained=True, device=device)
model.half() if device != torch.device('cpu') else True

# intialize data filepaths
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
# text_file_path = f"object_data/{timestamp}.txt"
# f = open(text_file_path, "a")
video_file_path = f"video_data_raw/{timestamp}.mp4"
out = cv2.VideoWriter(video_file_path, fourcc, 20, (640, 480))

# Use persistent session for faster requests
session = requests.Session()

cv2.namedWindow('Display', cv2.WINDOW_NORMAL)
cv2.resizeWindow('Display', 640, 480)

while True:
    try:
        # Fetch image from ESP32
        response = session.get(url, timeout=2)
        image_data = np.frombuffer(response.content, np.uint8)
        frame = cv2.imdecode(image_data, cv2.IMREAD_COLOR)
        if frame is None:
            continue  # Skip if image decoding fails

        # Convert BGR to RGB before inference
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = model(frame_rgb)

        # Convert annotated image from RGB to BGR before displaying
        annotated_image = cv2.cvtColor(results.render()[0], cv2.COLOR_RGB2BGR)

        # Show results
        cv2.imshow('Display', annotated_image)
        out.write(frame_rgb)
        
        # save detected objects to txt file
        # detections = results.pandas().xyxy[0]
        # for _, row in detections.iterrows():
        #     f.write(f"{row['name']} : {row['confidence']:.3f} ; {datetime.datetime.now()}\n")

        # Exit condition
        if cv2.waitKey(1) == 27:
            break

    except (requests.RequestException, cv2.error) as e:
        print(f"Error: {e}")
        time.sleep(0.5)  # Reduce spam on errors

# Cleanup
# f.close()
out.release()
cv2.destroyAllWindows()
session.close()