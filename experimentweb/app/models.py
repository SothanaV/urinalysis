import json
from django.db import models

class ExperimentResult(models.Model):
    create_time = models.DateTimeField(auto_now_add=True, blank=True)
    i = models.CharField(max_length=3000)
    v = models.CharField(max_length=3000)

    def set_v(self, v):
        self.v = json.dumps(v)

    def get_v(self):
        return json.loads(self.v)

    def set_i(self, i):
        self.i = json.dumps(i)

    def get_i(self):
        return json.loads(self.i)