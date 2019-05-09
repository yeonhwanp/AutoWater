import sqlite3
from datetime import datetime

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must send a POST request."

    try:
        override = request["form"]["override"]
        if override == "OFF":
            override = (False, True)
        elif override == "ON":
            override = (True, False)
        else:
            override = (False, False)
    except:
        return "You need to supply the override state."

    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    c.execute("""INSERT INTO pump_override VALUES (?, ?, ?)""", (*override, datetime.now()))
    conn.commit()
    conn.close()

    return "Override state updated: {}".format(str(override))