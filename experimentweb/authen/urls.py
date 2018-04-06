from django.urls import path
from authen.views import login
from django.contrib.auth.views import logout
urlpatterns = [
    path('login/', login, name='login'),
    path('logout/', logout, {'next_page': '/'}, name='logout'),
]