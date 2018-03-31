from rest_framework import routers
from app.viewsets import DeviceViewSet, TaskViewSet
router = routers.DefaultRouter()
router.register(r'device', DeviceViewSet, 'device')
router.register(r'task', TaskViewSet, 'task')
