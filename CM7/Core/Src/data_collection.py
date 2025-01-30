import serial
import time
import csv
import keyboard

# Ctrl+S = stop collecting data
# Ctrl+O = hand open
# Ctrl+C = hand close

SERIAL_PORT = "COM3"
BAUD_RATE = 115200
SAVE_FILE = "emg_data_1.csv"

# Initialize tracking of hand states
hand_state = "Unknown"
running = True

def update_hand_state(event):
    global hand_state, running
    if event.name == "o" and keyboard.is_pressed("ctrl"):
        hand_state = "Open"
    elif event.name == "c" and keyboard.is_pressed("ctrl"):
        hand_state = "Closed"
    elif event.name == "s" and keyboard.is_pressed("ctrl"):
        running = False  # Stop data collection
        print("\nStopping data collection...")

keyboard.on_press(update_hand_state)

# Open serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Open CSV file and write headers
with open(SAVE_FILE, mode="w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(["Timestamp ", "Channel ", "EMG Value ", "Hand State "])

    try:
        while running:
            # readline -> reads a line from serial port
            # decode -> converts byte to string
            # strip -> removes whitespace, \r and \n
            line = ser.readline().decode().strip()
            if line:
                # Does STM32 send data as: "<channel>,<value>"? - check this
                parts = line.split(",")
                if len(parts) == 2:
                    channel = parts[0].strip()
                    emg_value = parts[1].strip()

                    writer.writerow([time.time(), channel, emg_value, hand_state])
                    print(f"Channel {channel}: {emg_value} | Hand: {hand_state}")

    except KeyboardInterrupt:
        print("\nLogging stopped manually.")

    finally:
        ser.close()
        print("Serial connection closed.")