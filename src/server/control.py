import sys
import sqlite3

def request_handler(request):
    page = open("__HOME__/finalproject/server/templates/control.html").read()
    mainjs = open("__HOME__/finalproject/server/static/js/main.js").read()
    maincss = open("__HOME__/finalproject/server/static/css/main.css").read()

    page = page.replace("/* main.js */", mainjs)
    page = page.replace("/* main.css */", maincss)

    return page
