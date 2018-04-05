from django.db import models
import json
# Create your models here.
class Result(models.Model):
	time = models.DateTimeField(auto_now_add=True,blank=True)
	read = models.CharField(max_length=9999)
	write = models.CharField(max_length=9999)
	dac = models.CharField(max_length=9999)