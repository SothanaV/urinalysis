import pandas as pd
import numpy as np

from sklearn.isotonic import IsotonicRegression
from scipy.ndimage.filters import gaussian_filter

from django.http import HttpResponse
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt
from app.models import ExperimentResult
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
    return HttpResponse(command)


def sensor_parse(s):
    s = s.strip()
    s = s.split(',')
    return [int(x) for x in s if x]

@csrf_exempt
def data_in(request):
    if request.method == 'POST':
        print(request.POST)
        print(request.POST['I'])
        print()
        print(request.POST['V'])
        print('='*40)
        print()
        i = sensor_parse(request.POST['I'])
        v = sensor_parse(request.POST['V'])
        inst = ExperimentResult.objects.create(i=i, v=v)
        res = "{} {}".format(inst.pk, inst.create_time)
        print(res)
        return HttpResponse(res)
    return HttpResponse(status=404)

def get_result(request, pk):
    if request.method == 'GET':
        inst = ExperimentResult.objects.get(pk=pk)
        i = inst.get_i()
        v = inst.get_v()
        df = pd.DataFrame.from_dict({'i': i, 'v': v})
        X = df.v
        Y = df.i
        n = 1000
        model = IsotonicRegression().fit(X, Y)
        X_ = np.linspace(df.v.min(), df.v.max(), n)
        mdf = pd.DataFrame.from_dict({'v':X_, 'i':model.predict(X_)})
        mdf.i = gaussian_filter(mdf.i, sigma=20)
        mdf.v = (5/4095)*mdf.v
        mdf.i = (4.2/4095)*mdf.i
        # mdf = (3.4/4095)*mdf
        mdf = mdf.round(3)
        context = mdf.to_dict(orient='list')
        context['id'] = inst.pk
        context['create_time'] = inst.create_time
        return render(request, 'get_result.html', context)