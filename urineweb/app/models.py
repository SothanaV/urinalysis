import json
from django.db import models

usermodel = get_user_model()

class Team(models.Model):
    title = models.CharField(max_length=64)

class UserInformation(models.Model):
    user = models.OneToOneField(usermodel, on_delete=models.PROTECT, related_name='userinfo')
    team = models.ForeignKey(Team, null=True, blank=True, on_delete=models.SET_NULL, related_name='members')

class Command(models.Model):
    vmin = models.IntegerField(default=0)
    vmax = models.IntegerField(default=800)
    vcc = models.IntegerField(default=3300)
    pw = models.IntegerField(default=50)
    t = models.IntegerField(default=100)
    a = models.IntegerField(default=50)
    e = models.IntegerField(default=10)
    loop = models.IntegerField(default=3)
 
class Device(models.Model):
    IDLE = '1'
    PENDING = '2'
    STATUS_CHOICE = [(IDLE, 'idle'), (PENDING, 'pending')]

    serial = models.CharField(max_length=10, blank=True, unique=True)
    passwd = models.CharField(max_length=8, blank=True)
    command = models.OneToOneField(Command, null=True, blank=True, on_delete=models.SET_NULL)
    team = models.ForeignKey(Team, null=True, blank=True, on_delete=models.SET_NULL)
    status = models.CharField(max_length=1, default=IDLE, choices=STATUS_CHOICE)

    def save(self, *args, **kwargs):
        if not self.pk:
            self.command = Command.objects.create()
            _max_length = Device._meta.get_field('serial').max_length
            while True:
                serial = get_random_string(length=_max_length).upper()
                if not Device.objects.filter(serial=serial).exists():
                    self.serial = serial
                    break
            _max_length = Device._meta.get_field('passwd').max_length
            while True:
                passwd = get_random_string(length=_max_length).upper()
                if not Device.objects.filter(passwd=passwd).exists():
                    self.passwd = passwd
                    break
        super(Device, self).save(*args, **kwargs)

class Task(models.Model):
    WAITING = '1'
    PENDING = '2'
    DONE = '3'
    ABORT = '4'
    STATUS_CHOICE = [(WAITING, 'waiting'), (PENDING, 'pending'), (DONE, 'done'), (ABORT, 'abort')]

    device = models.ForeignKey(Device, on_delete=models.PROTECT)
    command = models.ForeignKey(Command, blank=True, on_delete=models.PROTECT)
    create_time = models.DateTimeField(auto_now_add=True, blank=True)
    done_time = models.DateTimeField(null=True, blank=True)
    status = models.CharField(max_length=1, default=WAITING, choices=STATUS_CHOICE)
    ri = models.CharField(max_length=6000, null=True, blank=True) 
    rv = models.CharField(max_length=6000, null=True, blank=True) 

    def save(self, *args, **kwargs):
        if not self.pk and self.device.status == Device.IDLE:
            self.command = self.device.command
            super(Task, self).save(*args, **kwargs)
        elif self.status==Task.DONE and self.ri[0] != '[':
            self.ri = self.parse_to_json(self.ri)
            self.rv = self.parse_to_json(self.rv)
            self.done_time = timezone.now()
            self.device.status = Device.IDLE
            self.device.save()
            super(Task, self).save(*args, **kwargs)
        else:
            super(Task, self).save(*args, **kwargs)