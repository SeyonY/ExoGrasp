import serial
import struct
import time
import csv

SERIAL_PORT = "COM6"         # Update as needed
BAUD_RATE = 921600
DATA_SIZE = 4                # Number of uint16_t values per read
BYTES_TO_READ = DATA_SIZE * 2
SAVE_FILE = "emg_data.csv"

# Time intervals
TOGGLE_INTERVAL = 3.0        # Seconds between toggles
INITIAL_WAIT = 1.0           # Seconds before data collection starts

START_SEQUENCE = b'\xFF\xEE' # Start marker

def toggle_hand_state(current_state):
    """Toggle hand state between 'Open' and 'Closed'."""
    new_state = "Closed" if current_state == "Open" else "Open"
    print(f"Hand state toggled to: {new_state}")
    return new_state

def read_packet(ser):
    """Reads a packet starting from a valid start sequence."""
    while True:
        # Read one byte at a time until we find the start sequence
        byte = ser.read(1)
        if byte == b'\xEE':  
            next_byte = ser.read(1)
            if next_byte == b'\xFF':  # Confirm second byte
                data = ser.read(BYTES_TO_READ)
                if len(data) == BYTES_TO_READ:
                    return data  # Successfully read a full packet

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
                # Read data packet with start byte verification
                raw_data = read_packet(ser)

                if raw_data:
                    data_array = struct.unpack('<{}H'.format(DATA_SIZE), raw_data)

                    current_time = time.time()

                    # Check if it's time to toggle
                    if current_time >= next_toggle_time:
                        hand_state = toggle_hand_state(hand_state)
                        next_toggle_time = current_time + TOGGLE_INTERVAL
                        time.sleep(1)

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
                    # print(f"Hand: {hand_state}, "
                    #     f"Time: {current_time:.2f}, "
                    #     f"Ch1: {data_array[0]}, Ch2: {data_array[1]}, "
                    #     f"Ch3: {data_array[2]}, Ch4: {data_array[3]}")

        except KeyboardInterrupt:
            print("\nStopping data collection (KeyboardInterrupt).")
        finally:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()
