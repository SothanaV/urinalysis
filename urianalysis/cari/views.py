from django.shortcuts import render
from django.http import HttpResponse
from django.views.decorators.csrf import csrf_exempt
global command
from .models import Result
# Create your views here.
def rin(request):
	global command
	if request.method == 'POST':
		vstart = int(request.POST.get('vstart'))
		vstop = int(request.POST.get('vstop'))
		delay  = int(request.POST.get('delay'))
		opset   = int(request.POST.get('opset'))
		command = ("%04d,%04d,%04d,%04d"%(vstart,vstop,delay,opset))
		print(command)
		return render(request,'form_cari.html')
	else:
		print("ELSE")
		return render(request,'form_cari.html')

def wemos(request):
	global command
	#command = command
	return HttpResponse(command)

@csrf_exempt
def wemos2(request):
	if request.method == 'POST':
		print(request.POST['read'])
		print()
		print(request.POST['write'])
		print()
		print(request.POST['dac'])
		print('_'*40)
		print()
		read = sensor_parse(request.POST['read'])
		write = sensor_parse(request.POST['write'])
		dac = sensor_parse(request.POST['dac'])
        #i = sensor_parse(request.POST['I'])
        #v = sensor_parse(request.POST['V'])
        #inst = ExperimentResult.objects.create(i=i, v=v)
        #res = "{} {}".format(inst.pk, inst.create_time)
        #print(res)
		db = Result.objects.create(read=read, write=write, dac=dac)
		return HttpResponse("OK")
	return HttpResponse(status=404)
	#return HttpResponse("POST")
def sensor_parse(s):
    s = s.strip()
    s = s.split(',')
    #return [int(x) for x in s if x]
    return [float(x) for x in s if x]