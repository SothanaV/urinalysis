from django.contrib import admin
from app.models import Command, Device, Task
# Register your models here.
admin.site.register(Command)
admin.site.register(Device)
admin.site.register(Task)
