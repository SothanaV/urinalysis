from django.core.exceptions import ValidationError, PermissionDenied
from rest_framework import serializers
from app.models import Task, Device

class DeviceSerializer(serializers.ModelSerializer):
    class Meta:
        model = Device
        fields = ('id', 'serial', 'status')

class TaskSerializer(serializers.ModelSerializer):
    device = DeviceSerializer()
    class Meta:
        model = Task
        fields   = '__all__'
