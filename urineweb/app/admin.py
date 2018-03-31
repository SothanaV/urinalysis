from django.contrib import admin
from app.models import Command, Device, Task, Team, UserInformation
# Register your models here.
admin.site.register(Command)
admin.site.register(Device)
admin.site.register(Task)
admin.site.register(Team)
admin.site.register(UserInformation)