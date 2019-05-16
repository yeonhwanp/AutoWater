# Determines whether or the pump should be running
# Returns TRUE/FALSE depending on whether the pump should be running
# This is only used by the ESP32
import sys
import sqlite3
from datetime import datetime
import json
sys.path.append("__HOME__/finalproject/server")
from utils import seconds_since_midnight

schedule_db = "__HOME__/schedule.db"
sensors_db = "__HOME__/sensors.db"

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    override = c.execute("""SELECT * FROM pump_override ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(override) != 0:
        on, off, _ = override[0]
        if on and not off:
            return json.dumps({"intent": "TRUE"})
        if off and not on:
            return json.dumps({"intent": "FALSE"})
    
    conn_sensors = sqlite3.connect(sensors_db)
    c_sensors = conn_sensors.cursor()
    # get the latest sensor reading
    last_reading = c_sensors.execute("""SELECT * FROM readings ORDER BY time DESC LIMIT 1""").fetchall()
    if len(last_reading) != 0:
        _, _, moisture, _ = last_reading[0]
        # get the moisture bounds
        moisture_min = c_sensors.execute("""SELECT * FROM moisture_min ORDER BY time DESC LIMIT 1""").fetchall()
        if len(moisture_min) != 0:
            moisture_min, _ = moisture_min[0]
            if moisture < moisture_min:
                return json.dumps({"intent": "TRUE"})

        moisture_max = c_sensors.execute("""SELECT * FROM moisture_max ORDER BY time DESC LIMIT 1""").fetchall()
        if len(moisture_max) != 0:
            moisture_max, _ = moisture_max[0]
            if moisture > moisture_max:
                return json.dumps({"intent": "FALSE"})

    schedules = c.execute("""SELECT * FROM pump_schedule ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(schedules) != 0:
        schedule = schedules[0][0]
    else:
        return json.dumps({"intent": "FALSE"})

    if schedule == "":
        return json.dumps({"intent": "FALSE"})

    cur_seconds = seconds_since_midnight()
    for segment in schedule.split(","):
        start, end = map(int, segment.split(":"))
        if start <= cur_seconds <= end:
            return json.dumps({"intent": "TRUE"})

    return json.dumps({"intent": "FALSE"})