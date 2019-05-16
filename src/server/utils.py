import sqlite3 
from datetime import datetime, timedelta
schedule_db = "__HOME__/schedule.db" # DB that holds the strings for the schedule
sensors_db = "__HOME__/sensors.db"   # DB that holds the readings from the sensors
camera_db = "__HOME__/camera.db"     # DB that holds the camera ip addresses

def create_databases():
    """
    Creates the schedule and sensor readings databases.
    """
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(schedule_db)
    c = conn.cursor()
    # pump_schedule: schedule text, update_time (to get the most recent schedule)
    c.execute("""CREATE TABLE IF NOT EXISTS pump_schedule (schedule text, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS lamp_schedule (schedule text, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS pump_override (pump_on boolean, pump_off boolean, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS lamp_override (lamp_on boolean, lamp_off boolean, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS pump_status (is_on boolean, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS lamp_status (is_on boolean, update_time timestamp);""")
    c.execute("""CREATE TABLE IF NOT EXISTS update_frequency (freq real, update_time timestamp);""")
    conn.commit()
    conn.close()

    # Create the sensor readings database
    conn = sqlite3.connect(sensors_db)
    c = conn.cursor()
    c.execute("""CREATE TABLE IF NOT EXISTS readings (temperature real, humidity real, moisture real, time timestamp)""")
    c.execute("""CREATE TABLE IF NOT EXISTS moisture_min (moisture real, time timestamp)""")
    c.execute("""CREATE TABLE IF NOT EXISTS moisture_max (moisture real, time timestamp)""")
    conn.commit()
    conn.close()

    # Create the camera photos database
    conn = sqlite3.connect(camera_db)
    c = conn.cursor()
    c.execute("""CREATE TABLE IF NOT EXISTS addresses (ip text, time timestamp)""")
    conn.commit()
    conn.close()

def seconds_since_midnight():
    now = datetime.now() - timedelta(hours=4)
    seconds_since = (now - now.replace(hour=0, minute=0, second=0, microsecond=0)).total_seconds()
    return seconds_since

def request_handler(request):
    create_databases()
    return "OK"
