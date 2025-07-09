import cv2
import os
import time

def is_usb_camera(device):
    try:
        cap = cv2.VideoCapture(device)
        if not cap.isOpened():
            return False
        cap.release()
        return True
    except Exception:
        return False

def find_first_usb_camera():
    video_devices = [os.path.join('/dev', dev) for dev in os.listdir('/dev') if dev.startswith('video')]
    for dev in video_devices:
        if is_usb_camera(dev):
            return dev
    return None

def get_max_resolution(cap):
    # 尝试设置最大分辨率
    max_width = 0
    max_height = 0
    
    # 常见分辨率列表，从大到小测试
    test_resolutions = [
        (3840, 2160), (2560, 1440), (1920, 1080),
        (1280, 720), (1024, 768), (800, 600),
        (640, 480), (320, 240)
    ]
    
    for width, height in test_resolutions:
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
        actual_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        actual_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        
        if actual_width >= width and actual_height >= height:
            max_width = actual_width
            max_height = actual_height
            break
    
    return max_width, max_height

# Initialize camera
video_device = find_first_usb_camera()
cap = cv2.VideoCapture(video_device, cv2.CAP_V4L2)  # 使用V4L2驱动以获得更多控制
if not cap.isOpened():
    exit(-1)

print("Open usb camera successfully")

# 获取并设置最大分辨率
max_width, max_height = get_max_resolution(cap)
if max_width == 0 or max_height == 0:
    print("Warning: Could not determine max resolution, using default 640x480")
    max_width, max_height = 640, 480

cap.set(cv2.CAP_PROP_FRAME_WIDTH, max_width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, max_height)

# 打印实际分辨率
actual_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
actual_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
print(f"Camera resolution set to: {actual_width}x{actual_height}")

# 尝试设置最大FOV（某些相机支持）
try:
    cap.set(cv2.CAP_PROP_ZOOM, 0)  # 0表示无缩放，最大FOV
    cap.set(cv2.CAP_PROP_AUTOFOCUS, 0)  # 禁用自动对焦（如果不需要）
except:
    pass

# Configure other camera settings
codec = cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')
cap.set(cv2.CAP_PROP_FOURCC, codec)
cap.set(cv2.CAP_PROP_FPS, 30)

# Create output directory if it doesn't exist
if not os.path.exists("./detect_res"):
    os.makedirs("./detect_res")

# Capture interval in seconds
capture_interval = 1
last_capture_time = time.time()
file_count = 0

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to capture frame")
            break
        
        current_time = time.time()
        
        # Check if it's time to capture a new image
        if current_time - last_capture_time >= capture_interval:
            file_count += 1
            filename = f"data/{file_count}.png"
            cv2.imwrite(filename, frame)
            print(f"Saved image: {filename} ({frame.shape[1]}x{frame.shape[0]})")
            last_capture_time = current_time
        
        # Press 'q' to quit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    # Release resources
    cap.release()
    cv2.destroyAllWindows()