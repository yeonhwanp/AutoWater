import sqlite3
import json

schedule_db = "__HOME__/schedule.db"

def request_handler(request):
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    freq = c.execute("""SELECT * FROM update_frequency ORDER BY update_time DESC LIMIT 1""").fetchall()

    if len(freq) != 0:
        freq, _ = freq[0]
    else:
        freq = 3000

    return json.dumps({"freq": freq})