from flask import Flask, render_template, request, jsonify,redirect, url_for
from flask_socketio import SocketIO, send, emit
import json

global command

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

@app.route('/data')
def render_simdata():
	return render_template('simdata.html')
@app.route('/log/<v>/<c>')
def log(v,c):
	voltage = float(data['v'])
	current = float(data['c'])
	socketio.emit('volt',voltage)
	socketio.emit('current',current)
	socketio.emit('s2cS',{'v':voltage,'c':current})
@socketio.on('c2s')
def recievedata(data):
	data = json.loads(data)
	voltage = float(data['v'])
	current = float(data['c'])
	socketio.emit('volt',voltage)
	socketio.emit('current',current)
	socketio.emit('s2cS',{'v':voltage,'c':current})
	print(data)

@app.route('/graph')
def graph():
	return render_template('graph.html')

@app.route('/graph2')
def graph2():
	return render_template('graphd3.html')

@app.route('/formin',methods=['GET',"POST"])
def forms():
	global command
	if request.method == 'POST':
		vmin = int(request.form['vmin'])
		vmax = int(request.form['vmax'])
		pw = int(request.form['pw'])
		t = int(request.form['t'])
		a = int(request.form['a'])
		e = int(request.form['vmin'])
		loop = int(request.form['loop'])
		command = ("%04d,%04d,%04d,%04d,%04d,%04d,%04d,"%(vmin,vmax,pw,t,a,e,loop))
		print(command)
		#return redirect(url_for('form'))
		return render_template('form.html')
	else:
		print("ELSE")
		#return redirect(url_for('form'))
		return render_template('form.html')

@app.route('/form')
def up():
	return render_template('form.html')
@app.route('/esp')
def esp():
	global command
	command = command
	print(command)
	return(command)