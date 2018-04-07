import json
import pandas as pd
import numpy as np

from sklearn.isotonic import IsotonicRegression
from sklearn.pipeline import make_pipeline
from sklearn.ensemble import RandomForestRegressor
from sklearn.preprocessing import PolynomialFeatures
from sklearn.externals import joblib
from scipy.ndimage.filters import gaussian_filter

from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.decorators import login_required

from app.models import ExperimentResult


_CONC_MODEL = joblib.load('static/conc_model.pkl')

vmin = 0
vmax = 800
vcc = 3300
pw = 50
t = 100
a = 25
e = 5
loop = 5
c2w = 'ready'
command = "hello"

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

# def start(request):
#     global c2w
#     c2w = 'start'
#     return redirect('/formin/')

# def ready(request):
#     global c2w
#     # pass
#     return HttpResponse(c2w)

# def start(request):
#     global c2w
#     if request.method == 'POST':
#         c2w = 'start'
#         return HttpResponse(status=200)

def get_data(request):
    global command
    return HttpResponse(command)

def sensor_parse(s):
    s = s.strip()
    s = s.split(',')
    return [int(x)/100 for x in s if x]

def get_state(request):
    global c2w
    if request.method == 'GET':
        return HttpResponse(c2w)

@csrf_exempt
def data_in(request):
    global c2w, vmin,vmax,vcc,pw,t,a,e,loop
    if request.method == 'POST':
        print(request.POST)
        print(request.POST['I'])
        print()
        print(request.POST['V'])
        print('='*40)
        print()
        i = sensor_parse(request.POST['I'])
        v = sensor_parse(request.POST['V'])
        inst = ExperimentResult.objects.create(i=i, v=v,
            vmin = vmin,
            vmax = vmax,
            vcc = vcc,
            pw = pw,
            t = t,
            a = a,
            e = e,
            loop = loop,
            cell=130,
        )
        res = "{} {}".format(inst.pk, inst.create_time)
        print(res)
        c2w = 'ready'
        return HttpResponse(res)
    return HttpResponse(status=404)

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
        return HttpResponse(status=200)
        
def render_base(request):
    return render(request,'base.html')

def find_peak(df):
    return df[df.v==0.47].i.max()

def set_param(request):
    global command, c2w
    global vmin,vmax,vcc,pw,t,a,e,loop
    if request.method == 'POST':
        values = json.loads(request.body.decode('utf-8'))
        vmin = int(values.get('vmin'))
        vmax = int(values.get('vmax'))
        vcc  = int(values.get('vcc'))
        pw   = int(values.get('pw'))
        t    = int(values.get('t'))
        a    = int(values.get('a'))
        e    = int(values.get('e'))
        loop = int(values.get('loop'))
        command = ("%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,"%(vmin,vmax,vcc,pw,t,a,e,loop))
        c2w = 'start'
        return HttpResponse(command+c2w, status=200)

@login_required
def get_result(request, pk=-1):
    latest = ExperimentResult.objects.latest('pk')
    if request.method == 'GET':
        context = dict()
        context['i'] = 'null'
        context['v'] = 'null'
        context['vmin'] = 0
        context['vmax'] = 800
        context['vcc']= 3300
        context['pw'] = 50
        context['t'] = 100
        context['a'] = 25
        context['e'] = 5
        context['loop'] = 5
        if pk == -1:
            inst = latest
        else:
            inst = ExperimentResult.objects.filter(pk=pk)
        if isinstance(inst, ExperimentResult) or inst.exists():
            if not isinstance(inst, ExperimentResult):
                inst = inst[0]
            i = inst.get_i()
            v = inst.get_v()
            try:
                df = pd.DataFrame.from_dict({'i': i, 'v': v})
                df = df.sort_values('v').reset_index()
                mdf = df

                # 
                # Randomforeest 
                # 
                # df = df[(df.i>0.003)&(df.i<0.004)]
                X = df.v.values.reshape([df.v.shape[0], 1])
                Y = df.i
                n = 1000
                model = make_pipeline(\
                            PolynomialFeatures(5), 
                            RandomForestRegressor(n_estimators=100, min_samples_split=100, random_state=1)\
                        ).fit(X, Y)
                X_ = np.linspace(df.v.min(), df.v.max(), n)
                mdf = pd.DataFrame.from_dict({'v':X_, 'i':model.predict(X_.reshape(-1, 1))})
                mdf.i = gaussian_filter(mdf.i, sigma=20)
                mdf = mdf.round(2)


                mdf.v = (mdf.v/4095)*5.0 
                mdf.i = ((mdf.i/4095)*5.0)/(0.033)
                mdf.v = round(mdf.v,2)
                mdf = mdf[(mdf.v>0.05)&(mdf.v<0.8)]

                #
                # find urine
                #
                peak = find_peak(mdf)

                # 
                # Tune float precision here
                # 
                print(peak, inst.cell)
                urine = '{:.4f}'.format(_CONC_MODEL.predict( [[inst.cell, peak]] )[0])

                # 
                # filter region
                # 
                mdf = mdf[(mdf.v>0.25) & (mdf.v<0.7)]
                _context = mdf.to_dict(orient='list')
                mapping = {'t': 'period',
                    'vmin': 'start voltage',
                    'vmax': 'stop voltage',
                    'vcc': 'circuit voltage',
                    'pw': 'pulse width',
                    'a': 'amplitude',
                    'e':  'step',
                    'loop': 'loop',
                    'cell': 'conductivity'
                }
                _params = {mapping[k]: getattr(inst, k) for k in ['vmin', 'vmax', 'vcc', 'pw', 't', 'a', 'e', 'loop', 'cell']}
                context.update(_context)
                context['params'] = _params
                context['id'] = inst.pk
                context['urine'] = urine
                context['message'] = 'graph'
                context['create_time'] = inst.create_time

                return render(request, 'get_result.html', context)

            except ValueError as err:
                print(err)
                context['id'] = inst.pk
                context['message'] = 'Invalid submit data'
                return render(request, 'get_result.html', context)
        else:
            if pk == latest.pk+1:
                context['id'] = latest.pk+1
                context['message'] = 'input'
                return render(request, 'get_result.html', context)
            else:
                return redirect('/result/{}'.format(latest.pk+1))