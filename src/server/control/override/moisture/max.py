import sqlite3
import json
from datetime import datetime

sensors_db = "__HOME__/sensors.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must use a POST request."
    
    try:
        value = request["form"]["value"]
    except:
        return "You must supply a `value`."

    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    c.execute("""INSERT INTO moisture_max VALUES (?, ?)""", (value, datetime.now()))
    conn.commit()
    conn.close()

    return "Moisture maximum updated."