import sqlite3
import json

sensors_db = "__HOME__/sensors.db"

def request_handler(request):
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    moisture_min = c.execute("""SELECT * FROM moisture_min ORDER BY time DESC LIMIT 1""").fetchall()
    moisture_max = c.execute("""SELECT * FROM moisture_max ORDER BY time DESC LIMIT 1""").fetchall()

    if len(moisture_max) != 0:
        moisture_max, _ = moisture_max[0]
    else:
        moisture_max = 100

    if len(moisture_min) != 0:
        moisture_min, _ = moisture_min[0]
    else:
        moisture_min = 0

    return json.dumps({"max": moisture_max, "min": moisture_min})