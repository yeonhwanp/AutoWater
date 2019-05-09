# Returns the current status of the lamp

import sqlite3
import json

schedule_db = "__HOME__/schedule.db" # DB that holds the strings for the schedule

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    statuses = c.execute("""SELECT * FROM lamp_status ORDER BY update_time DESC LIMIT 1""").fetchall()
    if len(statuses) != 0:
        lamp_status, _ = statuses[0]
    else:
        return json.dumps({"status": "ERROR: NO STATUS"})

    return json.dumps({"status": "ON"}) if lamp_status else json.dumps({"status": "OFF"})
