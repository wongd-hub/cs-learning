from flask import Flask, render_template, request

# Tell framework to treat this file as a web app - pass the name of this file to Flask
app = Flask(__name__)

# Implement root route to call `index()`
@app.route("/")
def index():
    
    # Render template reads and renders HTML/template files
    return render_template("form.html")


@app.route("/greet")
def greet():
    name = request.args.get("name", "world")
    return render_template("greet.html", name = name)