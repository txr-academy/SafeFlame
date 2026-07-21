import serial
import matplotlib.pyplot as plt
import time

# Adjust COM port and baud rate as needed
ser = serial.Serial('COM3', 115200, timeout=1)

mq2_ppm, mq4_ppm, mq7_ppm = [], [], []
timestamps = []

plt.ion()
fig, ax = plt.subplots()

while True:
    line = ser.readline().decode('utf-8').strip()
    if line:
        print(line)  # debug print
        current_time = time.time()

        if line.startswith("MQ2 sensor"):
            try:
                ppm = float(line.split("=")[1])
                mq2_ppm.append(ppm)
                timestamps.append(current_time)
            except:
                pass

        elif line.startswith("MQ4 sensor"):
            try:
                ppm = float(line.split("=")[1])
                mq4_ppm.append(ppm)
                # align with same timestamp list
                if len(timestamps) > len(mq4_ppm):
                    mq4_ppm.append(ppm)
            except:
                pass

        elif line.startswith("MQ7 sensor"):
            try:
                ppm = float(line.split("=")[1])
                mq7_ppm.append(ppm)
                # align with same timestamp list
                if len(timestamps) > len(mq7_ppm):
                    mq7_ppm.append(ppm)
            except:
                pass

        # Plot live
        ax.clear()
        ax.plot(timestamps[:len(mq2_ppm)], mq2_ppm, label="MQ2 - LPG")
        ax.plot(timestamps[:len(mq4_ppm)], mq4_ppm, label="MQ4 - Methane")
        ax.plot(timestamps[:len(mq7_ppm)], mq7_ppm, label="MQ7 - CO")
        ax.set_xlabel("Time (s)")
        ax.set_ylabel("PPM Concentration")
        ax.set_title("Real-Time Gas Concentration")
        ax.legend()
        ax.grid(True)
        plt.pause(0.01)
