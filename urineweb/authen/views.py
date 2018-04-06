from django.conf import settings
from django.shortcuts import render
from django.contrib.auth import authenticate
from django.contrib.auth import login as auth_login
from django.contrib.auth.forms import (
    AuthenticationForm, PasswordChangeForm, PasswordResetForm, SetPasswordForm,
)
from django.http import JsonResponse, HttpResponse, HttpResponseRedirect

# Create your views here.
_HOME_URL = settings.ROOT_URL

def login(request):
    if request.method == 'POST':
        username = request.POST.get('username', None)
        password = request.POST.get('password', None)
        if username and password:
            user = authenticate(username=username, password=password)
            if user:
                auth_login(request, user) 
                redirect = request.POST.get('next', _HOME_URL)
                if redirect == '//':
                    redirect = _HOME_URL
                return HttpResponseRedirect(redirect)

        form = AuthenticationForm(request)
        return render(request, 'registration/login.html', {
                'form':form, 
                'next':request.GET.get('next', _HOME_URL),
                'invalid': True,
                'username': username,
            }
        )
    else:
        form = AuthenticationForm(request)
        return render(request, 'registration/login.html', {
                'form':form,
                'next':request.GET.get('next', _HOME_URL)
            })

    return HttpResponse(status=403)
