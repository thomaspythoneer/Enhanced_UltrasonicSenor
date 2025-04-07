import serial, csv, time
from datetime import datetime

ser = serial.Serial('COM3', 9600)
time.sleep(2)

filename = f"ultrasonic_dataset_{int(time.time())}.csv"
with open(filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Angle", "Sensor", "Distance_cm", "Timestamp_ms", "Logged_At"])

    try:
        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                parts = line.split(',')
                if len(parts) == 4:
                    now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                    writer.writerow(parts + [now])
                    print("Logged:", parts)
    except KeyboardInterrupt:
        print("Stopped logging.")
