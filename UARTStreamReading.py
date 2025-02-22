import serial
import struct
import time
import csv

SERIAL_PORT = "COM4"         # Update as needed
BAUD_RATE = 921600
DATA_SIZE = 4                # Number of uint16_t values per read
BYTES_TO_READ = DATA_SIZE * 2
SAVE_FILE = "emg_data.csv"

# Time intervals
TOGGLE_INTERVAL = 2.0        # Seconds between toggles
INITIAL_WAIT = 1.0          # Seconds before data collection starts

def toggle_hand_state(current_state):
    """Toggle hand state between 'Open' and 'Closed'."""
    new_state = "Closed" if current_state == "Open" else "Open"
    print(f"Hand state toggled to: {new_state}")
    return new_state

def main():
    # Initial wait before starting
    print(f"Please wait for {INITIAL_WAIT} seconds before data collection starts...")
    time.sleep(INITIAL_WAIT)
    print("Starting data collection now.")

    # Open serial port
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

    # Initialize hand state
    hand_state = "Open"

    # Open CSV file and write headers
    with open(SAVE_FILE, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["Timestamp", "Channel1", "Channel2", "Channel3", "Channel4", "Hand State"])

        start_time = time.time()
        next_toggle_time = start_time + TOGGLE_INTERVAL

        try:
            while True:
                # Read the bytes from serial (4 * 2 bytes = 8 total)
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                data_array = line.split(",")  # Split by comma
                if len(data_array) == 4:

                    current_time = time.time()

                    # Check if it's time to toggle
                    if current_time >= next_toggle_time:
                        hand_state = toggle_hand_state(hand_state)
                        next_toggle_time = current_time + TOGGLE_INTERVAL

                    # Write row to CSV
                    writer.writerow([
                        current_time,
                        data_array[0],
                        data_array[1],
                        data_array[2],
                        data_array[3],
                        hand_state
                    ])

                    # Print to console (optional debug)
                    print(f"Time: {current_time:.2f}, "
                        f"Ch1: {data_array[0]}, Ch2: {data_array[1]}, "
                        f"Ch3: {data_array[2]}, Ch4: {data_array[3]} | "
                        f"Hand: {hand_state}")

        except KeyboardInterrupt:
            print("\nStopping data collection (KeyboardInterrupt).")
        finally:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()
