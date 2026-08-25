from flask import Flask

app = Flask(__name__)

@app.route("/")
def my_server():
    return "Mock server says Hi!"

if __name__ == "__main__":
    app.run(port=5000)