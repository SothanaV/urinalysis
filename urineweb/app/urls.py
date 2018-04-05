from django.urls import path
from app.views import start, get_command, submit_result
urlpatterns = [
    path('mycommand/<slug:serial>/', get_command),
    path('submit/<int:pk>/', submit_result),
]