import pandas as pd
import numpy as np

from sklearn.isotonic import IsotonicRegression
from sklearn.pipeline import make_pipeline
from sklearn.ensemble import RandomForestRegressor
from sklearn.preprocessing import PolynomialFeatures
from scipy.ndimage.filters import gaussian_filter

from django.http import HttpResponse
from django.shortcuts import render,redirect
from django.views.decorators.csrf import csrf_exempt
from app.models import ExperimentResult
# Create your views here.
global command
vmin = 0
vmax = 800
vcc = 3300
pw = 50
t = 100
a = 25
e = 5
loop = 5
c2w = 'ready'
def formin(request):
    global command
    global vmin,vmax,vcc,pw,t,a,e,loop
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
        # return render(request,'form.html')
    # else:
        # print("ELSE")
    context = {  
        'vmin' : vmin,
        'vmax' : vmax,
        'vcc' : vcc,
        'pw' : pw,
        't' : t,
        'a' : a,
        'e' : e,
        'loop' : loop,
    }
    return render(request,'form.html', context)

def start(request):
    global c2w
    c2w = 'start'
    return redirect('/formin/')
def ready(request):
    global c2w
    # pass
    return HttpResponse(c2w)
def get_data(request):
    global command
    return HttpResponse(command)


def sensor_parse(s):
    s = s.strip()
    s = s.split(',')
    return [int(x)/100 for x in s if x]

@csrf_exempt
def data_in(request):
    global c2w
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
        c2w = 'ready'
        return HttpResponse(res)
    return HttpResponse(status=404)
'''
def get_result(request, pk):
    if request.method == 'GET':
        inst = ExperimentResult.objects.get(pk=pk)
        i = inst.get_i()
        v = inst.get_v()
        print(len(i))
        print(len(v))
        df = pd.DataFrame.from_dict({'i': i, 'v': v})
        print(df.shape)
        df = df.sort_values('v').reset_index()
        mean = df.i.mean()
        std = df.i.std()
        # df = df[abs(df.i-mean)<(0.16*std)]
        mdf = df
        print(df.v.shape)
        df=df[df.i<450]
        X = df.v.values.reshape([df.v.shape[0], 1])
        Y = df.i
        
        # model = IsotonicRegression().fit(X, Y)     #
        model = make_pipeline(\
                    PolynomialFeatures(5), 
                    RandomForestRegressor(n_estimators=200, min_samples_split=2)\
                ).fit(X, Y)
        print(X.shape)
        n = 1000
        X_ = np.linspace(df.v.min(), df.v.max(), n)
        mdf = pd.DataFrame.from_dict({'v':X_, 'i':model.predict(X_.reshape(-1, 1))})
        mdf.i = gaussian_filter(mdf.i, sigma=20)      #
        mdf = mdf.round(2)
        mdf.v = (mdf.v/4095)*5.0 
        mdf.i = ((mdf.i/4095)*5.0)/(33*10^5)
        mdf.v = round(mdf.v,2)
        context = mdf.to_dict(orient='list')
        context['id'] = inst.pk
        context['create_time'] = inst.create_time
        print(context)
        print()
        return render(request, 'get_result.html', context)
        #return HttpResponse(context)
'''
def render_base(request):
    return render(request,'base.html')

def get_result(request, pk):
    if request.method == 'GET':
        inst = ExperimentResult.objects.get(pk=pk)
        i = inst.get_i()
        v = inst.get_v()
        df = pd.DataFrame.from_dict({'i': i, 'v': v})
        # df = df[df.i<448]
        #mean = df.i.mean()
        #std = df.i.std()
        # df = df[abs(df.i-mean)<(1*std)]
        df = df.sort_values('v').reset_index()
        mdf = df

        # print(df)
        #df=df[df.i<70]
        
        # 
        # Randomforeest 
        # 
        # df = df[(df.i>0.003)&(df.i<0.004)]
        X = df.v.values.reshape([df.v.shape[0], 1])
        # X = df.v.values.reshape([df.v.shape[0], 1])
        Y = df.i
        n = 1000
        model = make_pipeline(\
                    PolynomialFeatures(5), 
                    RandomForestRegressor(n_estimators=100, min_samples_split=100)\
                ).fit(X, Y)
        X_ = np.linspace(df.v.min(), df.v.max(), n)
        mdf = pd.DataFrame.from_dict({'v':X_, 'i':model.predict(X_.reshape(-1, 1))})
        # mdf = pd.DataFrame.from_dict({'v':X, 'i':Y.reshape([df.v.shape[0], 1])})
        mdf.i = gaussian_filter(mdf.i, sigma=20)
        mdf = mdf.round(2)


        mdf.v = (mdf.v/4095)*5.0 
        mdf.i = ((mdf.i/4095)*5.0)/(0.033)
        mdf.v = round(mdf.v,2)
        mdf = mdf[(mdf.v>0.05)&(mdf.v<0.8)]
        # mdf = mdf[(mdf.i>0.003)&(mdf.i<0.004)]
        print(mdf.v.max())
        context = mdf.to_dict(orient='list')
        context['id'] = inst.pk
        context['create_time'] = inst.create_time
        return render(request, 'get_result.html', context)
