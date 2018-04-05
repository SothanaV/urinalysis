from django.conf.urls import url
from . import views


urlpatterns = [
	url(r'^rin/', views.rin, name='rin'),
	url(r'^wemos/', views.wemos, name='wemos'),
	url(r'^wemos2/', views.wemos2, name='wemos2'), 
]