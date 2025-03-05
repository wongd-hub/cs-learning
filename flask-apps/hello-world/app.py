from flask import Flask, render_template, request

# Tell framework to treat this file as a web app - pass the name of this file to Flask
app = Flask(__name__)

# Implement root route to call `index()`
@app.route("/", methods=["GET", "POST"])
def index():

    if request.method == "POST":
        # Fix a bug where if the user inputs a blank name, "hello, " is printed.
        name = request.form.get("name", "world")
        return render_template("greet.html", name=name)

    return render_template("form.html")