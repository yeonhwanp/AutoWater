# Gets the current schedule planned for the pump, returned as a JSON
# This is used by the control server to display the current
#   schedule to the user
import sys
import sqlite3
import json
sys.path.append("__HOME__/finalproject/server")

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    schedules = c.execute("""SELECT * FROM pump_schedule ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(schedules) != 0:
        schedule = schedules[0][0]
    else:
        return json.dumps({"schedule":[]})

    segments = []
    for segment in schedule.split(","):
        start, end = map(int, segment.split(":"))
        segments.append({
            "start": start,
            "end": end
        })
    
    return json.dumps({"schedule": segments})