#!/usr/bin/env python3

import sys
import signal
import os
import time
import serial
from datetime import datetime

def signal_handler(signal, frame):
    print("\nCTRL+C detected, exiting...")
    sys.exit(0)

def extract_number(data):
    """从接收数据中提取 xx.xx 格式的小数值"""
    if '.' in data:
        try:
            # 找到 . 的位置
            dot_index = data.index('.')
            before = data[dot_index - 2 : dot_index]
            after = data[dot_index + 1 : dot_index + 3]

            if len(before) == 2 and len(after) == 2 and before.isdigit() and after.isdigit():
                full_number = f"{before}.{after}"
                return float(full_number)
        except Exception as e:
            print(f"Error extracting number: {e}")
    return None

def serialTest():
    uart_dev = "/dev/ttyS1"
    baudrate = 115200

    print(f"Testing UART device: {uart_dev} at {baudrate} baud")

    if not os.path.exists(uart_dev):
        print(f"Device {uart_dev} does not exist.")
        return -1

    try:
        ser = serial.Serial(
            port=uart_dev,
            baudrate=baudrate,
            timeout=1
        )
        print(f"Serial port {uart_dev} opened successfully.")

        last_extract_time = time.time()
        print("Waiting for data... (Format should be 'label:number')")

        try:
            while True:
                if ser.in_waiting > 0:
                    received_data = ser.readline().decode('utf-8', errors='ignore').strip()
                    if received_data:
                        print(f"Raw received: {received_data}")
                        
                        # 提取数字
                        number = extract_number(received_data)
                        print(number)
                        if number is not None:
                            # print(f"Extracted number: {number}")
                            
                            # 检查是否达到30秒间隔
                            current_time = time.time()
                            if current_time - last_extract_time >= 1:
                                print(f"[{datetime.now().strftime('%H:%M:%S')}] 30s interval value: {number}")
                                last_extract_time = current_time

                time.sleep(0.1)

        except KeyboardInterrupt:
            print("\nUser interrupted the process.")
        finally:
            ser.close()
            print("Serial port closed.")

    except serial.SerialException as e:
        print(f"Serial port error: {e}")
        return -1

    return 0

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    ret = serialTest()
    if ret != 0:
        print("Test failed!")
    else:
        print("Test completed successfully!")