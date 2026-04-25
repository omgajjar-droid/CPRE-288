# this is for that the txt file data is 100% made up by me so it basically useless :)

import socket
import threading
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Button
import numpy as np
import os
import math

CHOICE = "file" # this is allows me to test it just based on data I created for live update rename it to "live"

# getting the file for data
filename = "sensor-scan.txt"
absolute_path = os.path.dirname(__file__)
full_path = os.path.join(absolute_path, filename)

HOST = "192.168.1.1"
PORT = 288

# setting up the plot 
fig, ax = plt.subplots()
fig.subplots_adjust(left=0.15, right=0.75, top=0.90, bottom=0.15)

# origin 0,0
ax.plot(0, 0, marker='o', color='blue', markersize=10) 



# how big is the grid 
ax.set_xlim(-20, 20)
ax.set_ylim(-10, 10)

ax.set_xlabel("X (cm)")
ax.set_ylabel("Y (cm)") 
ax.set_title("CyBot Sensor Scan")

# cybot 
# ax.annotate('CyBot', xy=(0, 0), xytext=(-0.5, -0.5))

# storing the data of angle and distance
angle_data = []
dist_data  = []

# for the GUI grids points 
x_points = []
y_points = []

# starting points for the cybot
robot_x = [0]
robot_y = [0]


cybot_socket = None
cybot        = None
stop_flag    = False

# for angle, dist in zip(angle_data, dist_data):
#     angle_rad = math.radians(angle)
#     x = dist * math.cos(angle_rad)
#     y = dist * math.sin(angle_rad)
#     x_points.append(x)
#     y_points.append(y)

ax_scan = fig.add_axes([0.25,0.02, 0.1, 0.06])
ax_stop = fig.add_axes([0.55,0.02, 0.1, 0.06])

# ax.plot(0, 0, marker='s', color='royalblue', markersize=14)

button_scan = Button(ax_scan, 'Scan', color='lightblue', hovercolor='lightgreen')
button_stop = Button(ax_stop, 'Stop', color='lightcoral', hovercolor='lightpink')

robot_marker, = ax.plot(0, 0, marker='s', color='royalblue', markersize=14)
# trail_line, = ax.plot([0], [0], 'b--', linewidth=1.5) // don't need it, unless what to see the path

# def on_scan(event):
#     robot_x.append(robot_x[-1] + 0.5)  # move right by 0.5m each click
#     robot_y.append(robot_y[-1] + 0.3)  # move up by 0.3m each click
#     robot_marker.set_data([robot_x[-1]], [robot_y[-1]])
#     # trail_line.set_data(robot_x, robot_y) // trail 
#     fig.canvas.draw()

# this is how I am taking the output for the input of the data of Angle, IR(for rn) and distance which is cooked ngl(it made up)
def parse_and_store(line):
    line = line.strip()
    if line.startswith("Angle:") and "IR:" in line and "Distance:" in line:
        try:
            parts = {p.split(":")[0].strip(): p.split(":")[1].strip()
                     for p in line.split("|") if ":" in p}
            deg  = float(parts.get("Angle", 0))
            ir   = float(parts.get("IR", 0))
            dist = float(parts.get("Distance", 0))
            angle_data.append(deg)
            dist_data.append(ir / 500.0)
            robot_x[-1] = dist 
            robot_y[-1] = dist     

        except Exception:
            pass

# the data goes into this txt file which then later be read live and update the gui 
def read_from_file():
    angle_data.clear()
    dist_data.clear()
    robot_x[0] = 0
    robot_y[0] = 0
    if os.path.isfile(full_path):
        print("Loading file...")
        with open(full_path, 'r') as f:
            for line in f:
                parse_and_store(line)
        print(f"Loaded {len(angle_data)} points")
    else:
        print(f"File not found: {full_path}")

# the connection between the cybot and the GUI 
def init_socket():
    global cybot_socket, cybot
    cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cybot_socket.connect((HOST, PORT))
    cybot = cybot_socket.makefile("rbw", buffering=0)
    print("Connected to CyBot!")

# overall this does everything for receiving and updating the GUI 
def receive_loop():
    global stop_flag
    try:
        file_object = open(full_path, 'w')
        angle_data.clear()
        dist_data.clear()
        print("Scanning...")

        while not stop_flag:
            rx_message = cybot.readline()
            line = rx_message.decode()
            file_object.write(line)
            parse_and_store(line)
            if line.strip() == "END":
                break

        file_object.close()
        print(f"Scan complete — {len(angle_data)} points")

    except Exception as e:
        print(f"Socket error: {e}")

# the scan button 
def on_scan(event):
    global stop_flag
    if CHOICE == "file":
        read_from_file()
        return
    
    x_points.clear()
    y_points.clear()
   
    stop_flag = False
    try:
        if cybot is None:
            init_socket()
        cybot.write("m\n".encode())  # send scan command to CyBot
        thread = threading.Thread(target=receive_loop, daemon=True)
        thread.start()
        print("Scan command sent to CyBot")
    except Exception as e:
        print(f"Could not send scan: {e}")
        print("Scanning...")

# the stop button 
def on_stop(event):
    global stop_flag
    stop_flag = True
    try:
        if cybot is not None:
            cybot.write("p\n".encode())  # send stop command to CyBot
            print("Stop command sent to CyBot")
    except Exception as e:
        print(f"Could not send stop: {e}")
        print("Scan stopped")

# this is how I get the object to display on the GUI 
def draw_object(x, y, label):
    ax.plot(x, y, marker='s', color='red', markersize=5, zorder=5)
    ax.text(x, y + 0.15, label, ha='center', fontsize=8, color='darkred')
    fig.canvas.draw()

button_scan.on_clicked(on_scan)
button_stop.on_clicked(on_stop)

# about everything updating and how it displays on the GUI for corrdinates of x and y and robot poistion moving
def update(frame):
    if angle_data and dist_data:
        x_points.clear()
        y_points.clear()
        for angle, dist in zip(angle_data, dist_data):
            angle_rad = math.radians(angle)
            x_points.append(dist * math.cos(angle_rad))
            y_points.append(dist * math.sin(angle_rad))
        ax.cla()
        ax.set_xlim(-20, 20)
        ax.set_ylim(-10, 10)
        ax.set_xlabel("X (cm)")
        ax.set_ylabel("Y (cm)")
        ax.set_title("CyBot Sensor Scan")
        ax.grid(True)
        ax.plot(0, 0, marker='o', color='blue', markersize=10)  
        ax.plot(robot_x[-1], robot_y[-1], marker='s',           
                color='royalblue', markersize=14)
        ax.scatter(x_points, y_points, color='green')




# draw_object(1.0, 2.0, 'obstacle')
# draw_object(-1.0, 1.5, 'wall')
# draw_object(0.5, 3.5, 'unknown')

# this is how the cybot move and the update of the GUI works
# some things to knows is invterval is how fast it updates the data on GUI it 100ms
# cache frame is faulty data I think honestly i pick this line form mathlplot 
# save count it remembe the frames 
ani = FuncAnimation(fig, update, interval=100, cache_frame_data=False, save_count=200)
ax.grid(True)
# ax.scatter(x_points, y_points, color='green')
plt.show()