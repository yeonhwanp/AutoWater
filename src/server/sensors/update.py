import sys
import sqlite3
from datetime import datetime
sys.path.append("__HOME__/finalproject/server")

sensors_db = "__HOME__/sensors.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must use a POST request."

    try:
        temperature = float(request["form"]["temp"])
        humidity = float(request["form"]["humidity"])
        moisture = float(request["form"]["moisture"])
    except:
        return "You need to supply temp, humidity, and moisture."
    
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    c.execute("""INSERT INTO readings VALUES (?, ?, ?, ?)""",
        (temperature, humidity, moisture, datetime.now()))
    conn.commit()
    conn.close()

    return "Saved readings: [temp={}, humid={}, moist={}]".format(
        temperature, humidity, moisture)

