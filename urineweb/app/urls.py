from django.urls import path
from app.views import homepage, start, get_command, submit_result
urlpatterns = [
    path('mycommand/<slug:serial>/', get_command),
    path('submit/<int:pk>/', submit_result),
    path('start/<slug:serial>/', start),
    path('', homepage)
]