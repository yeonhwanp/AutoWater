import sys
import sqlite3
sys.path.append("__HOME__/finalproject/server")

sensors_db = "__HOME__/sensors.db"

def request_handler(request):
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    readings = c.execute("""SELECT * FROM readings ORDER BY time DESC LIMIT 1""").fetchall()
    conn.close()

    output = {
        "temp": str(readings[0][0]),
        "humid": str(readings[0][1]),
        "moist": str(readings[0][2])
    }

    return json.dumps(output)
