from django.shortcuts import render
from django.http import HttpResponse
from django.views.decorators.csrf import csrf_exempt

# Create your views here.
global command
command = "hello"
def formin(request):
	global command
	if request.method == 'POST':
		vmin = int(request.POST.get('vmin'))
		vmax = int(request.POST.get('vmax'))
		vcc  = int(request.POST.get('vcc'))
		pw   = int(request.POST.get('pw'))
		t    = int(request.POST.get('t'))
		a    = int(request.POST.get('a'))
		e    = int(request.POST.get('e'))
		loop = int(request.POST.get('loop'))
		command = ("%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,"%(vmin,vmax,vcc,pw,t,a,e,loop))
		print(command)
		return render(request,'form.html')
	else:
		print("ELSE")
		return render(request,'form.html')

def get_data(request,read_v,read_i):
	global command
	v = read_v.split
	print(v)
	return HttpResponse(command)

@csrf_exempt
def data_in(request):
	if request.method=="POST":
		print(request.POST)
	return HttpResponse(request,"Hi")