import sys
import sqlite3

def request_handler(request):
    page = open("__HOME__/finalproject/server/templates/control.html").read()
    return page
