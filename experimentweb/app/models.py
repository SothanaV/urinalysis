import json
from django.db import models

class ExperimentResult(models.Model):
    create_time = models.DateTimeField(auto_now_add=True, blank=True)
    cell = models.IntegerField(default=130)
    vmin = models.IntegerField(default=0)
    vmax = models.IntegerField(default=800)
    vcc = models.IntegerField(default=3300)
    pw = models.IntegerField(default=50)
    t = models.IntegerField(default=100)
    a = models.IntegerField(default=50)
    e = models.IntegerField(default=10)
    loop = models.IntegerField(default=3)
    i = models.CharField(max_length=3000)
    v = models.CharField(max_length=3000)
    memo = models.TextField(default='')

    def set_v(self, v):
        self.v = json.dumps(v)

    def get_v(self):
        return json.loads(self.v)

    def set_i(self, i):
        self.i = json.dumps(i)

    def get_i(self):
        return json.loads(self.i)