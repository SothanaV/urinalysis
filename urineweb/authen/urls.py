from django.urls import path
from authen.views import login
urlpatterns = [
    path('login', login, name='login')
]