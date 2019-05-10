import sqlite3
import sys
from datetime import datetime
sys.path.append("__HOME__/finalproject/server")

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must use a POST request."

    try:
        pump_status = request["form"]["pump_status"] == "ON"
        lamp_status = request["form"]["lamp_status"] == "ON"
    except:
        return "You need to supply pump_status and lamp_status."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO lamp_status VALUES (?, ?)""", (lamp_status, datetime.now()))
    c.execute("""INSERT INTO pump_status VALUES (?, ?)""", (pump_status, datetime.now()))
    conn.commit()
    conn.close()

    return "LAMP: {}/PUMP: {}".format(lamp_status, pump_status)