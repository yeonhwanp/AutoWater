# Determines whether or the pump should be running
# Returns TRUE/FALSE depending on whether the pump should be running
# This is only used by the ESP32
import sys
import sqlite3
from datetime import datetime
sys.path.append("__HOME__/finalproject/server")
from utils import seconds_since_midnight

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    schedules = c.execute("""SELECT * FROM pump_schedule ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(schedules) != 0:
        schedule = schedules[0][0]
    else:
        return "FALSE"

    cur_seconds = seconds_since_midnight()
    for segment in schedule.split(","):
        start, end = map(int, segment.split(":"))
        if start <= cur_seconds <= end:
            return "TRUE"

    return "FALSE"