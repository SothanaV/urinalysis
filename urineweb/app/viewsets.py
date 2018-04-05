import json
from django.contrib.auth import get_user_model
from django.contrib.auth.models import Group

from rest_framework import views, viewsets, mixins
from rest_framework.response import Response
from rest_framework.permissions import IsAuthenticated

from app.models import Device, Task
from app.serializers import DeviceSerializer, TaskSerializer

class DeviceViewSet(mixins.ListModelMixin, 
                        viewsets.GenericViewSet):
    serializer_class = DeviceSerializer
    permission_classes = (IsAuthenticated, )
    def get_queryset(self):
        return Device.objects.filter(team=self.request.user.userinfo.team)

class TaskViewSet(mixins.ListModelMixin,
                    viewsets.GenericViewSet):
    serializer_class = TaskSerializer
    permission_classes = (IsAuthenticated, )
    def get_queryset(self):
        return Task.objects.filter(device__team=self.request.user.userinfo.team)