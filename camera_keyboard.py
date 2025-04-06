import requests
import keyboard  # pip install keyboard
import time

BASE_URL = "http://localhost:18080"

controls = {
    'w': "/camera/rotate/up",
    's': "/camera/rotate/down",
    'a': "/camera/rotate/left",
    'd': "/camera/rotate/right",
    'e': "/camera/zoom/in",
    'q': "/camera/zoom/out",
    'r': "/camera/reset",
    'esc': "exit",

    'o': "/simulation/start",
    'p': "/simulation/stop",
}

def send_command(endpoint):
    try:
        res = requests.post(BASE_URL + endpoint)
        print(f"[{endpoint}] -> {res.status_code} - {res.json().get('message', '')}")
    except Exception as e:
        print(f"Error: {e}")


print("Camera control started (WASDQE + R to reset, ESC to quit)")
print("Press keys to send commands...")

while True:
    for key, route in controls.items():
        if keyboard.is_pressed(key):
            if route == "exit":
                print("Exiting...")
                break
            send_command(route)
            time.sleep(0.2)


