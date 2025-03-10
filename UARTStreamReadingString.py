import serial

# Update with your correct serial port
SERIAL_PORT = "COM7"  # Windows: "COM5", Linux/Mac: "/dev/ttyUSB0"
BAUD_RATE = 921600  # Match STM32 baud rate

# Open the serial port
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

print("Reading UART data continuously...\n")

try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()  # Read line & clean up
        if line:  # If line is not empty, print it
            print(line)

except KeyboardInterrupt:
    print("\nStopped by user.")
    ser.close()
