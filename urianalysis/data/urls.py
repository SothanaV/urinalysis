from django.conf.urls import url
from . import views


urlpatterns = [
	url(r'^formin/', views.formin, name='formin'),
	url(r'^wemos/(?P<read_v>\w+)/(?P<read_i>\w+)/?$', views.get_data, name='get_data'),
	url(r'^wemos2/', views.data_in, name='data_in'),
]