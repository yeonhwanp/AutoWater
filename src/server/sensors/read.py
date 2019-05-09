import sys
sys.path.append("__HOME__/finalproject")
from utils import create_databases

def request_handler(request):
    create_databases()
