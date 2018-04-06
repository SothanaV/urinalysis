"""urineweb URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.conf import settings
from django.urls import path, include
from django.conf.urls.static import static
from app.views import formin, data_in, get_result, get_data, render_base
urlpatterns = [
    path('formin/', formin, name='formin'),
    path('admin/', admin.site.urls),
    path('data/wemos2/', data_in),
    path('data/wemos/<int:read_v>/<int:read_i>', get_data),
    path('result/<int:pk>/', get_result, name='result'),
    path('base/', render_base,name='base'),
    path('', get_result, name='homepage'),
    path('auth/', include('authen.urls')),

] + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)\
    + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

