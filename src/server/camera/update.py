import sqlite3
from datetime import datetime

camera_db = "__HOME__/camera.db"

def request_handler(request):
    if request["method"] != "POST":
        return "You must use a POST request."
    
    try:
        ip = request["form"]["ip"]
    except:
        return "You must provide an `ip`."
    
    conn = sqlite3.connect(camera_db)
    c = conn.cursor()
    c.execute("""INSERT INTO addresses VALUES (?, ?)""", (ip, datetime.now()))
    conn.commit()
    conn.close()

    return "IP address stored."