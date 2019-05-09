import sqlite3
import json
from datetime import datetime

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    override_status = c.execute("""SELECT * FROM pump_override ORDER BY update_time DESC LIMIT 1""").fetchall()
    conn.commit()
    conn.close()

    if len(override_status) != 0:
        on, off, _ = override_status[0]
        if on and not off:
            return json.dumps({"override": "ON"})
        elif off and not on:
            return json.dumps({"override": "OFF"})
        else:
            return json.dumps({"override": "NONE"})

    return json.dumps({"override": "NONE"})