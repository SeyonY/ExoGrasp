use serialport::SerialPort;
use std::fs::OpenOptions;
use std::io::{self, Read, Write};
use std::time::{Duration, SystemTime, UNIX_EPOCH};

const SERIAL_PORT: &str = "COM4"; // Change as needed
const BAUD_RATE: u32 = 921600;
const DATA_SIZE: usize = 4; // Number of u16 values
const BYTES_TO_READ: usize = DATA_SIZE * 2; // 4 * 2 bytes = 8 total
const SAVE_FILE: &str = "emg_data.csv";

const TOGGLE_INTERVAL: Duration = Duration::from_secs(2);
const INITIAL_WAIT: Duration = Duration::from_secs(1);

fn toggle_hand_state(current_state: &str) -> String {
    let new_state = if current_state == "Open" { "Closed" } else { "Open" };
    println!("Hand state toggled to: {}", new_state);
    new_state.to_string()
}

fn main() {
    println!("Please wait for {:?} seconds before data collection starts...", INITIAL_WAIT);
    std::thread::sleep(INITIAL_WAIT);
    println!("Starting data collection now.");

    // Open serial port
    let mut port = match serialport::new(SERIAL_PORT, BAUD_RATE)
        .timeout(Duration::from_secs(1))
        .open()
    {
        Ok(port) => {
            println!("Serial port {} opened successfully!", SERIAL_PORT);
            port
        }
        Err(e) => {
            eprintln!("Failed to open serial port: {}", e);
            return;
        }
    };

    // Open CSV file and write headers
    let mut file = match OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(SAVE_FILE)
    {
        Ok(f) => f,
        Err(e) => {
            eprintln!("Failed to open CSV file: {}", e);
            return;
        }
    };

    // Write CSV header
    if let Err(e) = writeln!(file, "Timestamp,Channel1,Channel2,Channel3,Channel4,Hand State") {
        eprintln!("Failed to write header to CSV: {}", e);
        return;
    }

    let mut buffer = [0u8; BYTES_TO_READ];
    let mut hand_state = "Open".to_string();
    let start_time = SystemTime::now();
    let mut next_toggle_time = start_time + TOGGLE_INTERVAL;

    println!("Reading from serial port... Press Ctrl+C to exit.");

    loop {
        match port.read_exact(&mut buffer) {
            Ok(_) => {
                let data: Vec<u16> = buffer
                    .chunks_exact(2)
                    .map(|b| u16::from_le_bytes([b[0], b[1]]))
                    .collect();

                let current_time = SystemTime::now();
                let timestamp = current_time.duration_since(UNIX_EPOCH).unwrap().as_secs_f64();

                // Toggle hand state at interval
                if current_time >= next_toggle_time {
                    hand_state = toggle_hand_state(&hand_state);
                    next_toggle_time = current_time + TOGGLE_INTERVAL;
                }

                // Write to CSV
                if let Err(e) = writeln!(
                    file,
                    "{},{},{},{},{},{}",
                    timestamp, data[0], data[1], data[2], data[3], hand_state
                ) {
                    eprintln!("Failed to write to CSV: {}", e);
                }

                file.flush().unwrap(); // Ensure data is written immediately
            }
            Err(ref e) if e.kind() == io::ErrorKind::TimedOut => {
                // Ignore timeout errors (happens if no data arrives in time)
            }
            Err(e) => {
                eprintln!("Error reading from serial port: {}", e);
                break;
            }
        }
    }
}
