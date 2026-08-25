from flask import Flask
import sys

app1 = Flask("backend1")
app2 = Flask("backend2")
app3 = Flask("backend3")


@app1.route("/")
def server1():
    return "Response from backend 1"

@app2.route("/")
def server2():
    return "Response from backend 2"

@app3.route("/")
def server3():
    return "Response from backend 3"

if __name__ == "__main__":
    if sys.argv[1] == "1":
        app1.run(port=5001)
    elif sys.argv[1] == "2":
        app2.run(port=5002)
    elif sys.argv[1] == "3":
        app3.run(port=5003)
 