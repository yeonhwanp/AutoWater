import sys
import sqlite3
import json
from datetime import datetime, timedelta
sys.path.append("__HOME__/finalproject/server")

sensors_db = "__HOME__/sensors.db"

def request_handler(request):
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    one_day_ago = datetime.now() - timedelta(days=1)
    readings = c.execute("""SELECT * FROM readings WHERE time > ? ORDER BY time ASC""", (one_day_ago, )).fetchall()
    conn.close()

    temps = [{"t": reading[3], "y": reading[0]} for reading in readings]
    humids = [{"t": reading[3], "y": reading[1]} for reading in readings]
    moists = [{"t": reading[3], "y": reading[2]} for reading in readings]

    try:
        output = {
            "temps": temps,
            "humids": humids,
            "moists": moists
        }
    except:
        return "NO/BAD READINGS!"

    return json.dumps(output)
