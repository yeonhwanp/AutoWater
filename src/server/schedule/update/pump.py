import sys
import sqlite3
from datetime import datetime
sys.path.append("__HOME__/finalproject")

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must use a POST request."

    # Schedule format
    # time_start:time_end,time_start2:time_end2
    try:
        new_schedule = request["form"]["schedule"]
    except:
        return "You need to supply a schedule."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO pump_schedule VALUES (?, ?)""", (new_schedule, datetime.now()))
    conn.commit()
    conn.close()
    return "New pump schedule saved."