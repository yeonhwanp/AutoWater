import sqlite3
import json

camera_db = "__HOME__/camera.db"

def request_handler(request):
    conn = sqlite3.connect(camera_db)
    c = conn.cursor()
    ips = c.execute("""SELECT * FROM addresses ORDER BY time DESC LIMIT 1""").fetchall()
    conn.close()

    if len(ips) == 0:
        return json.dumps({"success": False, "address": "", "error": "NO STREAM"})
    
    ip, _ = ips[0]
    return json.dumps({"success": True, "address": ip})
