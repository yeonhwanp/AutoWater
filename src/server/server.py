from flask import Flask, render_template, request
import sqlite3
from datetime import datetime

app = Flask(__name__)

schedule_db = "schedule.db" # DB that holds the strings for the schedule
sensors_db = "sensors.db" # DB that holds the readings from the sensors
 
def seconds_since_midnight():
    now = datetime.now()
    seconds_since = (now - now.replace(hour=0, minute=0, second=0, microsecond=0)).total_seconds()
    return seconds_since

def create_database():
    """
    Creates the schedule database.
    """
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    # pump_schedule: schedule text, update_time (to get the most recent schedule)
    c.execute("""CREATE TABLE IF NOT EXISTS pump_schedule (schedule text, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS lamp_schedule (schedule text, update_time timestamp);""")
    conn.commit()
    conn.close()

    # Create the sensor readings database
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    c.execute("""CREATE TABLE IF NOT EXISTS readings (temperature real, humidity real, moisture real, time timestamp)""")
    conn.commit()
    conn.close()

@app.route("/")
def home():
    return render_template("index.html")

# Determines whether or not the pump should be on
@app.route("/schedule/pump")
def should_pump_be_on():
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    schedules = c.execute("""SELECT * FROM pump_schedule ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(schedules) != 0:
        schedule = schedules[0][0]
    else:
        return "FALSE"
    
    print("PUMP schedule:", schedule)
    
    cur_seconds = seconds_since_midnight()
    for segment in schedule.split(","):
        start, end = map(int, segment.split(":"))
        if start <= cur_seconds <= end:
            return "TRUE"

    return "FALSE"

@app.route("/schedule/lamp")
def should_lamp_be_on():
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    schedules = c.execute("""SELECT * FROM lamp_schedule ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(schedules) != 0:
        schedule = schedules[0][0]
    else:
        return "FALSE"
    
    print("LAMP schedule:", schedule)
    
    cur_seconds = seconds_since_midnight()
    for segment in schedule.split(","):
        start, end = map(int, segment.split(":"))
        if start <= cur_seconds <= end:
            return "TRUE"

    return "FALSE"

@app.route("/schedule/pump/update", methods=["POST"])
def update_pump_schedule():
    # Schedule format
    # time_start:time_end,time_start2:time_end2
    try:
        new_schedule = request.form.get("schedule")
    except:
        return "You need to supply a schedule."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO pump_schedule VALUES (?, ?)""", (new_schedule, datetime.now()))
    conn.commit()
    conn.close()
    return "New pump schedule saved."

@app.route("/schedule/lamp/update", methods=["POST"])
def update_lamp_schedule():
    try:
        new_schedule = request.form.get("schedule")
    except:
        return "You need to supply a schedule."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO lamp_schedule VALUES (?, ?)""", (new_schedule, datetime.now()))
    conn.commit()
    conn.close()
    return "New lamp schedule saved."

@app.route("/sensors/update", methods=["POST"])
def update_sensor_readings():
    try:
        temperature = float(request.form.get("temp"))
        humidity = float(request.form.get("humidity"))
        moisture = float(request.form.get("moisture"))
    except:
        return "You need to supply temp, humidity, and moisture."
    
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    c.execute("""INSERT INTO readings VALUES (?, ?, ?, ?)""",
        (temperature, humidity, moisture, datetime.now()))
    conn.commit()
    conn.close()
    return "Saved readings: [temp={}, humid={}, moist={}]".format(
        temperature, humidity, moisture)

@app.route("/sensors")
def get_sensor_readings():
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    readings = c.execute("""SELECT * FROM readings ORDER BY time DESC""").fetchall()
    conn.close()
    output = ""
    for reading in readings:
        # temp, humid, moist = map(str, reading)
        output += ",".join(map(str, reading))
        output += "\r\n"
    
    return output

if __name__ == "__main__":
    create_database()  # create the databases if necessary
    app.run(host="0.0.0.0", port=80, debug=True)