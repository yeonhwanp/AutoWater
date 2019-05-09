# Determines whether or not the lamp should be on
# Returns TRUE/FALSE depending on the desired state

import sys
import sqlite3
from datetime import datetime
import json
sys.path.append("__HOME__/finalproject/server")
from utils import seconds_since_midnight

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    override = c.execute("""SELECT * FROM lamp_override ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(override) != 0:
        on, off, _ = override[0]
        if on and not off:
            return json.dumps({"intent": "TRUE"})
        if off and not on:
            return json.dumps({"intent": "FALSE"})

    schedules = c.execute("""SELECT * FROM lamp_schedule ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(schedules) != 0:
        schedule = schedules[0][0]
    else:
        return json.dumps({"intent": "FALSE"})

    cur_seconds = seconds_since_midnight()
    for segment in schedule.split(","):
        start, end = map(int, segment.split(":"))
        if start <= cur_seconds <= end:
            return json.dumps({"intent": "TRUE"})

    return json.dumps({"intent": "FALSE"})