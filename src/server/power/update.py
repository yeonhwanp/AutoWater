import sqlite3
from datetime import datetime
import json

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must use a POST request"

    try:
        frequency = request["form"]["freq"]
    except:
        return "You must supply a `freq`."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO update_frequency VALUES (?, ?)""", (frequency, datetime.now()))
    conn.commit()
    conn.close()

    return "Frequency updated."