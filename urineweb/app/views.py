import json
from django.shortcuts import render
from django.core import serializers
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.core.exceptions import ObjectDoesNotExist
from app.models import Task, Device, Command


def _model_to_dict(instance):
    print(serializers.serialize('json', [ instance, ]))
    return json.loads(serializers.serialize('json', [ instance, ]))[0]

def get_command(request, serial):
    try:
        device = Device.objects.get(serial=serial)
        print(device.status)
        if device.status == Device.PENDING:
            device.status = Device.IDLE
            for task in Task.objects.filter(device=device, status=Task.PENDING):
                task.status = Task.ABORT
                task.save()
                print('abort task', task)
            device.save()

        qs = Task.objects.filter(device=device, status=Task.WAITING)
        if qs.exists():
            task = qs[0]
            task.status = Task.PENDING
            task.save()
            device.status = Device.PENDING
            device.save()
            res = _model_to_dict(device.command)['fields']
            res['id'] = task.pk
            return JsonResponse(res)
        else:
            return HttpResponse('IDLE', status=200) 
    except ObjectDoesNotExist as err:
        return HttpResponse('Invalid Device', status=404)

@csrf_exempt
def submit_result(request, pk):
    task = Task.objects.filter(pk=pk)
    device = Device.objects.filter(serial=request.POST['serial'])
    if not device.exists():
        return HttpResponse(' Not Found device serial')
    if not task.exists():
        return HttpResponse('Not found task id', status=404)
    
    task = task[0]
    device = device[0]
    if task.device == device:
        task.status = Task.DONE
        task.ri = request.POST['I']
        task.rv = request.POST['V']
        task.save()
        return HttpResponse(status=200)
    else:
        return HttpResponse(status=403)


def start(request, serial):
    if request.method == "GET":
        device = Device.objects.get(serial=serial)
        return render(request, 'app/start.html', {'device':device})

def homepage(request):
    if request.method == "GET":
        return render(request, 'app/homepage.html')