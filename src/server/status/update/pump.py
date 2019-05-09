import sqlite3
import sys
from datetime import datetime
sys.path.append("__HOME__/finalproject/server")

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    try:
        status = request["form"]["status"] == "ON"
    except:
        return "You need to supply a status."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO pump_status VALUES (?, ?)""", (status, datetime.now()))
    conn.commit()
    conn.close()

    return status