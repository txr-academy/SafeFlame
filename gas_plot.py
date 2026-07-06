import serial
import matplotlib.pyplot as plt
import csv
import time

# Configure your COM port and baud rate
ser = serial.Serial('COM3', 115200, timeout=1)

mq2_ppm, mq4_ppm, mq7_ppm = [], [], []
timestamps = []

# Prepare CSV logging
with open("gas_log.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["Timestamp", "MQ2_PPM", "MQ4_PPM", "MQ7_PPM"])

    plt.ion()
    fig, ax = plt.subplots()

    while True:
        line = ser.readline().decode('utf-8').strip()
        if line:
            print(line)  # debug print
            current_time = time.time()

            mq2_val, mq4_val, mq7_val = None, None, None

            # Parse MQ2
            if line.startswith("MQ2:"):
                try:
                    mq2_val = float(line.split("PPM=")[1])
                    mq2_ppm.append(mq2_val)
                except:
                    pass

            # Parse MQ4
            elif line.startswith("MQ4:"):
                try:
                    mq4_val = float(line.split("PPM=")[1])
                    mq4_ppm.append(mq4_val)
                except:
                    pass

            # Parse MQ7
            elif line.startswith("MQ7:"):
                try:
                    mq7_val = float(line.split("PPM=")[1])
                    mq7_ppm.append(mq7_val)
                except:
                    pass

            # Append timestamp only when at least one sensor value is parsed
            if mq2_val or mq4_val or mq7_val:
                timestamps.append(current_time)
                writer.writerow([current_time, mq2_val, mq4_val, mq7_val])
                csvfile.flush()

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
